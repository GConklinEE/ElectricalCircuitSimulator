using System.Numerics;
using System.Runtime.InteropServices;
using System.Transactions;

namespace SimulationEngineBindingsCSharp
{
  public class LinearCircuit: IDisposable
  {
    #region Constructors
    public LinearCircuit(nuint numComponents)
    {
      _linearCircuitPtr = createLinearCircuit(numComponents);
    }
    #endregion

    #region Modifiers
    public nuint AddResistor(nuint nodeS, nuint nodeD, double resistance)
    {
      return addResistor(_linearCircuitPtr, nodeS, nodeD, resistance);
    }

    public nuint AddInductor(nuint nodeS, nuint nodeD, double inductance)
    {
      return addInductor(_linearCircuitPtr, nodeS, nodeD, inductance);
    }

    public nuint AddCapacitor(nuint nodeS, nuint nodeD, double capacitance)
    {
      return addCapacitor(_linearCircuitPtr, nodeS, nodeD, capacitance);
    }

    public nuint AddGroundedVoltageSource(nuint nodeS, nuint nodeD, double voltage, double resistance)
    {
      return addGroundedVoltageSource(_linearCircuitPtr, nodeS, nodeD, voltage, resistance);
    }

    public void SetStopTime(double stopTime)
    {
      setStopTime(_linearCircuitPtr, stopTime);
    }

    public void SetTimeStep(double stepTime)
    {
      setTimeStep(_linearCircuitPtr, stepTime);
    }

    public void Initialize()
    {
      initialize(_linearCircuitPtr);
    }

    public bool Step()
    {
      return step(_linearCircuitPtr);
    }
    #endregion

    #region Observers
    public double GetTime()
    {
      return getTime(_linearCircuitPtr);
    }

    public double GetVoltage(nuint node)
    {
      return getVoltage(_linearCircuitPtr, node);
    }

    public double GetCurrent(nuint componentIndex)
    {
      return getCurrent(_linearCircuitPtr, componentIndex);
    }
    #endregion

    #region IDispose
    public void Dispose()
    {
      destroyLinearCircuit(_linearCircuitPtr);
    }
    #endregion

    #region Data Members
    IntPtr _linearCircuitPtr;
    #endregion

    #region P/Invoke
    [DllImport("SimulationEngineBindingsC")]
    private static extern IntPtr createLinearCircuit(nuint numComponents);
    [DllImport("SimulationEngineBindingsC")]
    private static extern void destroyLinearCircuit(IntPtr linearCircuitPtr);
    [DllImport("SimulationEngineBindingsC")]
    private static extern nuint addResistor(IntPtr linearCircuitPtr, nuint nodeS, nuint nodeD, double resistance);
    [DllImport("SimulationEngineBindingsC")]
    private static extern nuint addInductor(IntPtr linearCircuitPtr, nuint nodeS, nuint nodeD, double inductance);
    [DllImport("SimulationEngineBindingsC")]
    private static extern nuint addCapacitor(IntPtr linearCircuitPtr, nuint nodeS, nuint nodeD, double capacitance);
    [DllImport("SimulationEngineBindingsC")]
    private static extern nuint addGroundedVoltageSource(IntPtr linearCircuitPtr, nuint nodeS, nuint nodeD, double voltage, double resistance);
    [DllImport("SimulationEngineBindingsC")]
    private static extern void setStopTime(IntPtr linearCircuitPtr, double stopTime);
    [DllImport("SimulationEngineBindingsC")]
    private static extern void setTimeStep(IntPtr linearCircuitPtr, double timeStep);
    [DllImport("SimulationEngineBindingsC")]
    private static extern void initialize(IntPtr linearCircuitPtr);
    [DllImport("SimulationEngineBindingsC")]
    private static extern bool step(IntPtr linearCircuitPtr);
    [DllImport("SimulationEngineBindingsC")]
    private static extern double getTime(IntPtr linearCircuitPtr);
    [DllImport("SimulationEngineBindingsC")]
    private static extern double getVoltage(IntPtr linearCircuitPtr, nuint node);
    [DllImport("SimulationEngineBindingsC")]
    private static extern double getCurrent(IntPtr linearCircuitPtr, nuint componentIndex);
    #endregion
  }
}
