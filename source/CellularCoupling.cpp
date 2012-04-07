
/*! \file CellularCoupling.cpp
    \brief This file implements classes that implement cellular coupling.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "Grid.h"
#include "PopulationStatistics.h"
#include "CellularCoupling.h"

namespace Population
{
	namespace CouplingOperations
	{

		/// <summary>Datatype for storing iterators that walk the neighbour cells for each branch that execute coupling operation in tags.</summary>
		typedef Common::Data::GaSingleDimensionArray<Common::Grid::GaHyperBoxNeighbour> GaPartialHyperBoxNeighbour;

		// Prepares population for coupling operation
		void GaCellularCoupling::Prepare(GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const
		{
			input.GetPopulation()->GetTagManager().AddTag( ( (const GaCellularCouplingParams&)parameters ).GetNeighbourHyperBoxTagID(),
				Common::Data::GaSizableTagLifecycle<GaPartialHyperBoxNeighbour>( branchCount ) );

			GaCouplingOperation::Prepare( input, output, parameters, configuration, branchCount );
		}

		// Clears population from data used by the operation
		void GaCellularCoupling::Clear(GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const
		{
			input.GetPopulation()->GetTagManager().RemoveTag( ( (const GaCellularCouplingParams&)parameters ).GetNeighbourHyperBoxTagID() );

			GaCouplingOperation::Clear( input, output, parameters, configuration, branchCount );
		}

		// Updates population for coupling operation
		void GaCellularCoupling::Update(GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const
		{
			input.GetPopulation()->GetTagByID<GaPartialHyperBoxNeighbour>( ( (const GaCellularCouplingParams&)parameters ).GetNeighbourHyperBoxTagID() ).SetSize( branchCount );

			GaCouplingOperation::Update( input, output, parameters, configuration, branchCount );
		}

		// Execute coupling operation
		void GaCellularCoupling::Exec(GaChromosomeGroup& input,
				GaChromosomeGroup& output,
				const GaCellularCouplingParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const
		{
			GaPopulation& population = *input.GetPopulation();

			GaCouplingCounters counters( population, GADV_COUPLING_TIME );

			int branchID = branch->GetFilteredID();
			int branchCount = branch->GetBarrierCount();

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
				output.Clear();

			GaChromosomeTagGetter<Common::Grid::GaHyperBox> getHyperBox( parameters.GetHyperBoxTagID(), population.GetChromosomeTagManager() );
			GaChromosomeTagGetter<int> getRank( parameters.GetRankTagID(), population.GetChromosomeTagManager() );

			GaCrossoverStorageBuffer& crossoverBuffer = population.GetTagByID<GaPartialCrossoverBuffer>( parameters.GetCrossoverBuffersTagID() )[ branchID ];
			GaChromosomeGroup& parents = crossoverBuffer.GetParents();
			GaChromosomeGroup& offspring = crossoverBuffer.GetOffspring();

			GaBestChromosomesMatrix::GaCellMatrix::GaMatrix& bestChromosomes =
				population.GetTagByID<GaBestChromosomesMatrix>( parameters.GetBestIndividualsMatrixTagID() ).GetCells().GetMatrix();

			const int* gridSize = bestChromosomes.GetDimensionSizes();
			int dimensionCount = bestChromosomes.GetDimensionCount();

			Common::Grid::GaHyperBoxNeighbour& coords = population.GetTagByID<GaPartialHyperBoxNeighbour>( parameters.GetNeighbourHyperBoxTagID() )[ branchID ];
			coords.SetCoordinatesCount( dimensionCount );

			// get workload for this branch
			int start, count;
			branch->SplitWork( parameters.GetNumberOfOffsprings(), count, start );

			Common::Sorting::GaSortCriteriaConverter<GaChromosomeStorage*, Common::Sorting::GaAscendingSortingCriteria<int>, GaChromosomeTagGetter<int> > parentSort( getRank );

			int parentCount = configuration.GetMating().GetConfiguration().GetParentCount();
			for( int i = start + count - 1; i >= start; i-- )
			{
				GaChromosomeStorage* parent1 = &input[ i % input.GetCount() ];
				GaChromosomeStorage* parent2 = bestChromosomes[ getHyperBox( parent1 ) ].GetBest();

				parents.Add( parent1 );
				if( parent1 != parent2 )
					parents.Add( parent2 );

				// find best ranking chromosomes in neighbour cells and add them to crossover buffer
				// each iteration moves search one cell further from the cell in which selected chromosome is located.
				for( int level = 1; ; level++ )
				{
					// start visiting neighbour cells that are at certian distance
					coords.Begin( getHyperBox( parent1 ), gridSize, level );

					int processed = 0;
					do
					{
						processed++;

						// try to add the best chromosome from current cell to crossover buffer
						GaChromosomeStorage* chromosome = bestChromosomes[ coords.GetCoordinates() ].GetBest();
						if( chromosome )
							parents.AddSorted( chromosome, parentSort, 1 );

						// move to next cell
					} while( coords.Next() );

					// all visited cells are empty?
					if( !processed )
					{
						// try to add best chromosome in cell of selected chromosome to fill the buffer and restart search to closest neighbours
						if( parents.GetCount() < parentCount )
							parents.AddSorted( bestChromosomes[ getHyperBox( parent1 ) ].GetBest(), parentSort, 1 );

						level = 0;
					}

					if( parents.GetCount() == parentCount )
						break;
				}

				// produce offspring
				configuration.Mate( crossoverBuffer );

				// store offspring to result set
				for( int j = offspring.GetCount() - 1; j >= 0; j-- )
				{
					if( offspring[ j ].GetParent() == parent1 )
					{
						output.Add( &offspring[ j ] );
						offspring.Remove( j, true );
						break;
					}
				}

				// update operation counters
				counters.CollectCrossoverBufferCounters( crossoverBuffer );

				crossoverBuffer.Clear();
			}

			// update population statistics with new state of counters
			counters.UpdateStatistics();
		}

	} // CouplingOperations
} // Population
