/***********************************************************************************
Copyright (c) 2017, Michael Neunert, Markus Giftthaler, Markus Stäuble, Diego Pardo,
Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of ETH ZURICH nor the names of its contributors may be used
      to endorse or promote products derived from this software without specific
      prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL ETH ZURICH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************************/

#ifndef INCLUDE_CT_OPTCON_SOLVER_NLOPTCONSOLVERBASE_H_
#define INCLUDE_CT_OPTCON_SOLVER_NLOPTCONSOLVERBASE_H_

#include <ct/core/core.h>
#include <ct/optcon/problem/OptConProblem.h>


namespace ct{
namespace optcon{


/** \defgroup OptConSolver OptConSolver
 * Solver interface for finite horizon optimal control problems
 */
template <typename DERIVED, typename POLICY, typename SETTINGS, size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR = double>
class NLOptConSolver : public OptConSolver<DERIVED, POLICY, SETTINGS, STATE_DIM, CONTROL_DIM, SCALAR>{

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	static const size_t STATE_D = STATE_DIM;
	static const size_t CONTROL_D = CONTROL_DIM;

	typedef POLICY Policy_t;
	typedef SETTINGS Settings_t;
	typedef DERIVED Derived;
	typedef SCALAR Scalar_t;

	typedef OptConProblem<STATE_DIM, CONTROL_DIM, SCALAR> OptConProblem_t;


	NLOptConSolver() {}

	virtual ~NLOptConSolver(){}

	/**
	 * configures the solver
	 * */
	virtual void configure(const Settings_t& settings) = 0;

	virtual void prepare() = 0;

	virtual bool finishSolve() = 0;

	/**
	 * run a single iteration of the solver
	 * @return true if a better solution was found
	 */
	virtual bool runIteration() override = 0;

	/*!
	 * Set the initial guess used by the solver (not all solvers might support initial guesses)
	 */
	virtual void setInitialGuess(const Policy_t& initialGuess) = 0;

	/**
	 * solve the optimal control problem
	 * @return true if solve succeeded, false otherwise.
	 * */
	virtual bool solve()
	{
		bool solved = false;

		while (!solved)
		{
			prepare();
			solved = finishSolve();
		}

		return solved;
	}

	/**
	 * Get the optimized control policy to the optimal control problem
	 * @return
	 */
	virtual const Policy_t& getSolution() override { return nlocBackend_->getSolution(); }

	/**
	 * Get the optimized trajectory to the optimal control problem
	 * @return
	 */
	virtual const core::StateTrajectory<STATE_DIM, SCALAR> getStateTrajectory() const override { return nlocBackend_->getStateTrajectory(); }

	/**
	 * Get the optimal feedforward control input corresponding to the optimal trajectory
	 * @return
	 */
	virtual const core::ControlTrajectory<CONTROL_DIM, SCALAR> getControlTrajectory() const override { return nlocBackend_->getControlTrajectory(); }

	/**
	 * Get the time indices corresponding to the solution
	 * @return
	 */
	virtual const core::tpl::TimeArray<SCALAR>& getTimeArray() const override { return nlocBackend_->getTimeArray(); }



	/*!
	 * \brief Get the time horizon the solver currently operates on.
	 *
	 */
	virtual SCALAR getTimeHorizon() const override { return nlocBackend_->getTimeHorizon(); }


	/*!
	 * \brief Change the time horizon the solver operates on.
	 *
	 * This function does not need to be called if setOptConProblem() has been called
	 * with an OptConProblem that had the correct time horizon set.
	 */
	virtual void changeTimeHorizon(const SCALAR& tf) override { nlocBackend_->changeTimeHorizon(tf); }

	/*!
	 * \brief Change the initial state for the optimal control problem
	 *
	 * This function does not need to be called if setOptConProblem() has been called
	 * with an OptConProblem that had the correct initial state set
	 */
	virtual void changeInitialState(const core::StateVector<STATE_DIM, SCALAR>& x0) override { nlocBackend_->changeInitialState(x0); }

	/*!
	 * \brief Change the cost function
	 *
	 * This function does not need to be called if setOptConProblem() has been called
	 * with an OptConProblem that had the correct cost function
	 */
	virtual void changeCostFunction(const typename OptConProblem_t::CostFunctionPtr_t& cf) override { nlocBackend_->changeCostFunction(cf); }

	/*!
	 * \brief Change the nonlinear system
	 *
	 * This function does not need to be called if setOptConProblem() has been called
	 * with an OptConProblem that had the correct nonlinear system
	 */
	virtual void changeNonlinearSystem(const typename OptConProblem_t::DynamicsPtr_t& dyn) override { nlocBackend_->changeNonlinearSystem(dyn); }

	/*!
	 * \brief Change the linear system
	 *
	 * This function does not need to be called if setOptConProblem() has been called
	 * with an OptConProblem that had the correct linear system
	 */
	virtual void changeLinearSystem(const typename OptConProblem_t::LinearPtr_t& lin) override { nlocBackend_->changeLinearSystem(lin); }


	virtual SCALAR getCost() const override
	{
		{ return nlocBackend_->getCost(); }
	}


protected:
	std::shared_ptr<NLOCBackendBase> nlocBackend_;

};


}
}



#endif /* INCLUDE_CT_OPTCON_SOLVER_NLOPTCONSOLVERBASE_H_ */