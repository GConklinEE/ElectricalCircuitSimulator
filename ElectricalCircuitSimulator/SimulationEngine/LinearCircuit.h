#ifndef LINEARCIRCUIT_H
#define LINEARCIRCUIT_H

#include "CircuitComponent.h"
#include <vector>

using std::vector;

namespace SimulationEngine {

	class LinearCircuit {

		public:

			LinearCircuit(const int iNumComponents);

			~LinearCircuit();

			int addComponent(CircuitComponent* pCircuitComponent);
			void setStopTime(const double dStopTime);
			void setTimeStep(const double dTimeStep);
			double getTime() const;
			double getVoltage(const int iNode) const;
			double getCurrent(const int iComponentIndex) const;
			void initalize();
			bool step();

		private:

			CircuitComponent** m_oCircuitComponents;
			Matrix m_oConductanceMatrix;
			Matrix m_oSourceVector;
			Matrix m_oVoltageMatrix;
			PLU_Factorization m_oPLU_Factorization;
			vector<int> m_oNodeList;
			bool m_bHasGround;
			int m_iMaxComponentCount;
			int m_iComponentCount;
			int m_iNodeCount;
			double m_dStopTime;
			double m_dTimeStep;
			double m_dTime;
			bool m_bInitSim;
			bool m_bRunSim;
	};

}

#endif
