#ifndef GROUNDED_VOLTAGE_SOURCE_H
#define GROUNDED_VOLTAGE_SOURCE_H

#include "CircuitComponent.h"

namespace SimulationEngine {

	class GroundedVoltageSource : public CircuitComponent {

		public:

			GroundedVoltageSource(const int iNodeS, const int iNodeD, const double dVoltage, const double dResistance); // iNodeD is assumed to be ground

			void initalize(Matrix& oConductanceMatrix, const double dTimeStep);
			void step(Matrix& oSourceVector, const double dTimeStep);
			void postStep(Matrix& oVoltageMatrix, const double dTimeStep);
			void applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep) const;

		private:

			double m_dVoltage;
			double m_dResistance;
	};

}

#endif