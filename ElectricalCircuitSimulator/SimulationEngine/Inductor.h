#ifndef INDUCTOR_H
#define INDUCTOR_H

#include "CircuitComponent.h"

namespace SimulationEngine {

	class Inductor : public CircuitComponent {

		public:

			Inductor(const int iNodeS, const int iNodeD, const double m_dInductance);

			void initalize(Matrix& oConductanceMatrix, const double dTimeStep);
			void step(Matrix& oSourceVector, const double dTimeStep); // Trapezoidal integration
			void postStep(Matrix& oVoltageMatrix, const double dTimeStep);
			void applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep) const;

		private:

			double m_dInductance;
			double m_dVoltageDeltaT;
			double m_dVoltageDeltaTM1;
	};

}

#endif