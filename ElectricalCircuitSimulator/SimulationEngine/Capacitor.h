#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "CircuitComponent.h"

namespace SimulationEngine {

	class Capacitor : public CircuitComponent {

		public:

			Capacitor(const int iNodeS, const int iNodeD, const double m_dCapacitance);

			void initalize(Matrix& oConductanceMatrix, const double dTimeStep);
			void step(Matrix& oSourceVector); // Trapezoidal integration
			void postStep(Matrix& oVoltageMatrix);
			void applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep);

		private:

			double m_dCapacitance;
			double m_dVoltageDelta;
	};

}

#endif