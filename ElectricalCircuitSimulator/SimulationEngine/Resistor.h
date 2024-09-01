#ifndef RESISTOR_H
#define RESISTOR_H

#include "CircuitComponent.h"

namespace SimulationEngine {

	class Resistor : public CircuitComponent {

		public:

			Resistor(const int iNodeS, const int iNodeD, const double dResistance);

			void initalize(Matrix& oConductanceMatrix, const double dTimeStep);
			void postStep(Matrix& oVoltageMatrix, const double dTimeStep);
			void applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep) const;

		private:

			double m_dResistance;
	};

}

#endif