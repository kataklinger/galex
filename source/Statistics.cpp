
/*! \file Statistics.cpp
    \brief This file contains implementation of classes needed for tracking statistical information about populations and genetic algorithms.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "Statistics.h"

namespace Statistics
{

	// Sets dependency of history depth
	void GaValueHistoryBase::SetFreeHistoryDepth(bool free)
	{ 
		if( !free )
			SetHistoryDepth( _owner->GetHistoryDepth() );

		_freeHistoryDepth = free;
	}

	// Adds value to the list of dependant values
	void GaValueHistoryBase::AddDependant(GaValueHistoryBase* value)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, value != NULL, "value", "Dependant value must be specifed.", "Statistics" );
		GA_ARG_ASSERT( Common::Exceptions::GaArgumentException, !IsDependant( value ), "value", "Dependant value is already bound.", "Statistics" );

		_dependants.insert( value );
	}

	// Removes value from the list of dependant values
	void GaValueHistoryBase::RemoveDependant(GaValueHistoryBase* value)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, value != NULL, "value", "Dependant value must be specifed.", "Statistics" );

		if( !IsDependant( value ) )
			return;

		_dependants.erase( value );
	}

	// Adds value to the list of dependencies values
	void GaValueHistoryBase::AddDependency(GaValueHistoryBase* value)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, value != NULL, "value", "Dependency must be specifed.", "Statistics" );
		GA_ARG_ASSERT( Common::Exceptions::GaArgumentException, !IsDependant( value ), "value", "This value is already bound to the specified dependency.", "Statistics" );

		_dependencies.insert( value );
		value->AddDependant( this );
	}

	// Removes value from the list of dependencies values
	void GaValueHistoryBase::RemoveDependency(GaValueHistoryBase* value)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, value != NULL, "value", "Dependency must be specifed.", "Statistics" );

		if( IsDependency( value ) )
		{
			_dependencies.erase( value );
			value->RemoveDependant( this );
		}
	}

	// Removes all dependencies for the value
	void GaValueHistoryBase::RemoveAllDependencies()
	{
		for( std::set<GaValueHistoryBase*>::iterator it = _dependencies.begin(); it != _dependencies.end(); ++it )
			( *it )->RemoveDependant( this );
	}

	// Sets combiner object
	void GaValueHistoryBase::SetCombiner(GaValueCombiner* combiner)
	{
		GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _evaluator == NULL, "Cannot set combiner for value which is evaluated automatically.", "Statistics" );
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, combiner != NULL, "combiner", "Combiner must be specifed.", "Statistics" );

		_combiner = combiner;
	}

	// Sets evaluator object
	void GaValueHistoryBase::SetEvaluator(GaValueEvaluator* evaluator)
	{
		GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _combiner == NULL, "Cannot set evaluator for value which is created as independent value.", "Statistics" );
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, evaluator != NULL, "evaluator", "Evaluator must be specifed.", "Statistics" );

		// removes dependencies established by the previous evaluator
		RemoveAllDependencies();

		// establishe new dependencies and reevaluate value useing new evaluator
		_evaluator = evaluator; 
		_evaluator->Bind( this );
		Evaluate();
	}

	// Inserts new value to the statistics
	void GaStatistics::AddValue(int id,
		GaValueHistoryBase* value)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, value != NULL, "value", "Value object must be specified.", "Statistics" );
		GA_ARG_ASSERT( Common::Exceptions::GaArgumentException, !IsExist( id ), "id", "A value with specified value is already registered.", "Statistics" );

		_values[ id ] = value;
	}

	// Removes value from the statistics
	void GaStatistics::RemoveValue(int id)
	{
		STLEXT::hash_map<int, GaValueHistoryBase*>::iterator it = _values.find( id );
		if( it == _values.end() )
			return;

		// unbind value from it evaluation dependencies
		it->second->RemoveAllDependencies();

		// delete value
		delete it->second;
		_values.erase( it );
	}

	// Binds value to its evaluation dependency.
	void GaStatistics::BindValues(GaValueHistoryBase* value,
		int dependencyID)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, value != NULL, "value", "Value for binding must be specified.", "Statistics" );
		value->AddDependency( &GetValue( dependencyID ) );
	}

	// Sets history depth for all values in the group
	void GaStatistics::SetHistoryDepth(int depth)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, depth >= 0, "depth", "Depth cannot have negative value.", "Statistics" );

		// set history depth for all values that are bound to preferred history dept
		for( STLEXT::hash_map<int, GaValueHistoryBase*>::iterator it = _values.begin(); it != _values.end(); ++it )
		{
			if( !it->second->IsFreeHistoryDepth() )
				it->second->SetHistoryDepth( depth );
		}

		_historyDepth = depth;
	}

} // Statistics
