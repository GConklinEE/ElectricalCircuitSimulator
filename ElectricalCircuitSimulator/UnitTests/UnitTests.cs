using ElectricalCircuitSimulator;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using SimulationEngineWrapper;

namespace UnitTests
{
    #region Helper Functions

    public class AssertAction() {
        public static void VerifyAssert(Action oAction, string sFailMessage)
        {
            bool bSuccess = false;

            try
            {
                oAction();
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail(sFailMessage);
                }
            }
        }

        public static void CheckAssert(Action oAction, string sFailMessage)
        {
            try
            {
                oAction();
            }
            catch
            {
                Assert.Fail(sFailMessage);
            }
        }
    }

    public class STATestMethod : TestMethodAttribute
    {
        public override TestResult[] Execute(ITestMethod testMethod)
        {
            if (Thread.CurrentThread.GetApartmentState() == ApartmentState.STA)
                return Invoke(testMethod);

            TestResult[] oResult = Array.Empty<TestResult>();
            Thread thread = new Thread(() => oResult = Invoke(testMethod));
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();
            thread.Join();
            return oResult;
        }

        private TestResult[] Invoke(ITestMethod testMethod)
        {
            return new[] { testMethod.Invoke(null) };
        }
    }

    #endregion

    #region Test UI

    [TestClass]
    public class TestMainWindow
    {
        [STATestMethod]
        public void TestMainWindowFunctionality()
        {
            MainWindow oMainWindow = new MainWindow(0);
            
            RoutedEventArgs oRoutedEventArgs = new RoutedEventArgs();
            TextChangedEventArgs oTextChangedEventArgs = new TextChangedEventArgs(TextBox.TextChangedEvent, UndoAction.None);
            MouseButtonEventArgsTest oMouseButtonEventArgsTest = new MouseButtonEventArgsTest(Mouse.PrimaryDevice, MouseButton.Left, MouseButtonState.Pressed);
           
            IndexTextBlock oIndexTextBlock = new IndexTextBlock();
            TextBox oTextBox = new TextBox();
            Button oButton = new Button();

            const int iFilesLoaded = 5;
            int iIndex = iFilesLoaded;

            Assert.IsTrue(oIndexTextBlock.Index == 0, "Index property returned unexpected value after being configured with blank constructor!");
            oIndexTextBlock = new IndexTextBlock(iIndex);
            Assert.IsTrue(oIndexTextBlock.Index == iIndex, "Index property returned unexpected value after being configured with constructor!");
            iIndex++;
            oIndexTextBlock.Index = iIndex;
            Assert.IsTrue(oIndexTextBlock.Index == iIndex, "Index property returned unexpected value after being configured with property set method!");

            oMainWindow = new MainWindow(true);
            if (oMainWindow != null) {
                if (oMainWindow.UI_Error)
                {
                    Assert.Fail("UI error in MainWindow!");
                }

                oIndexTextBlock.Index = 2;
                AssertAction.CheckAssert(() => oMainWindow.TextBox_MouseLeftButtonDown(oIndexTextBlock, oMouseButtonEventArgsTest), "Failed to run TextBox_MouseLeftButtonDown!");

                oIndexTextBlock.Index = iFilesLoaded;
                AssertAction.VerifyAssert(() => oMainWindow.TextBox_MouseLeftButtonDown(oIndexTextBlock, oMouseButtonEventArgsTest), "Loaded more files than expected!");
                oIndexTextBlock.Index = iFilesLoaded - 1;
                AssertAction.CheckAssert(() => oMainWindow.TextBox_MouseLeftButtonDown(oIndexTextBlock, oMouseButtonEventArgsTest), "Loaded less files than expected!");

                oTextBox.Text = "5";
                AssertAction.CheckAssert(() => oMainWindow.StepSizeTextBox_TextChanged(oTextBox, oTextChangedEventArgs), "Failed to run StepSizeTextBox_TextChanged!");
                oTextBox.Text = "-1";
                AssertAction.CheckAssert(() => oMainWindow.StepSizeTextBox_TextChanged(oTextBox, oTextChangedEventArgs), "Failed to run StepSizeTextBox_TextChanged!");
                oTextBox.Text = "A";
                AssertAction.CheckAssert(() => oMainWindow.StepSizeTextBox_TextChanged(oTextBox, oTextChangedEventArgs), "Failed to run StepSizeTextBox_TextChanged!");

                oTextBox.Text = "5";
                AssertAction.CheckAssert(() => oMainWindow.StepNumTextBox_TextChanged(oTextBox, oTextChangedEventArgs), "Failed to run StepNumTextBox_TextChanged!");
                oTextBox.Text = "-1";
                AssertAction.CheckAssert(() => oMainWindow.StepNumTextBox_TextChanged(oTextBox, oTextChangedEventArgs), "Failed to run StepNumTextBox_TextChanged!");
                oTextBox.Text = "A";
                AssertAction.CheckAssert(() => oMainWindow.StepNumTextBox_TextChanged(oTextBox, oTextChangedEventArgs), "Failed to run StepNumTextBox_TextChanged!");

                AssertAction.CheckAssert(() => oMainWindow.Screen_MouseDown(oIndexTextBlock, oMouseButtonEventArgsTest), "Failed to run Screen_MouseDown!");
                AssertAction.VerifyAssert(() => oMainWindow.CloseButton_Click(oButton, oRoutedEventArgs), "Expected running CloseButton_Click to fail, it did not!");
                AssertAction.CheckAssert(() => oMainWindow.MinimizeButton_Click(oButton, oRoutedEventArgs), "Failed to run MinimizeButton_Click!");

                // Event toggles states, so call it twice
                AssertAction.CheckAssert(() => oMainWindow.SimulateButton_Click(oButton, oRoutedEventArgs), "Failed to run SimulateButton_Click!");
                AssertAction.CheckAssert(() => oMainWindow.SimulateButton_Click(oButton, oRoutedEventArgs), "Failed to run SimulateButton_Click!");
            }
        }
    }

    #endregion

    #region Test Circuit Simulation Blocks

    [TestClass]
    public class TestCircuitSimulationBlocks
    {
        [TestMethod]
        public void TestMatrix()
        {
            int iRow;
            int iColumn;

            Matrix oMatrix = new Matrix();
            oMatrix = new Matrix(2, 6);
            AssertAction.VerifyAssert(() => oMatrix = new Matrix(0, 5), "Expected 'Matrix dimensions must be positive and non-zero!' error, did not get it!");
            AssertAction.VerifyAssert(() => oMatrix = new Matrix(6, 0), "Expected 'Matrix dimensions must be positive and non-zero!' error, did not get it!");
            AssertAction.VerifyAssert(() => oMatrix = new Matrix(-6, 6), "Expected 'Matrix dimensions must be positive and non-zero!' error, did not get it!");
            AssertAction.VerifyAssert(() => oMatrix = new Matrix(6, -6), "Expected 'Matrix dimensions must be positive and non-zero!' error, did not get it!");

            oMatrix = new Matrix(5, 5);
            Assert.IsTrue(oMatrix.getNumRows() == 5, "Expected 'Incorrect matrix row count! Expected 5' did not get it!");
            Assert.IsTrue(oMatrix.getNumColumns() == 5, "Expected 'Incorrect matrix column count! Expected 5' did not get it!");
            oMatrix.printMatrix();
            AssertAction.VerifyAssert(() => oMatrix.setValue(3, 5, 11), "Expected 'Location beyond dimensions of matrix!' error, did not get it!");
            AssertAction.VerifyAssert(() => oMatrix.setValue(5, 3, 11), "Expected 'Location beyond dimensions of matrix!' error, did not get it!");
            AssertAction.VerifyAssert(() => oMatrix.getValue(3, 5), "Expected 'Location beyond dimensions of matrix!' error, did not get it!");
            AssertAction.VerifyAssert(() => oMatrix.getValue(5, 3), "Expected 'Location beyond dimensions of matrix!' error, did not get it!");
            oMatrix.setValue(2, 3, 11);
            Assert.IsTrue((int)Math.Round(oMatrix.getValue(2, 3)) == 11, "Incorrect matrix value! Expected 11");
            for (iRow = 0; iRow < oMatrix.getNumRows(); iRow++)
            {
                for (iColumn = 0; iColumn < oMatrix.getNumColumns(); iColumn++)
                {
                    if ((iRow != 2) && (iColumn != 3))
                    {
                        int tmp = (int)Math.Round(oMatrix.getValue(iRow, iColumn));
                        Assert.IsTrue((int)Math.Round(oMatrix.getValue(iRow, iColumn)) == 0, "Incorrect matrix value! Expected 0m got " + tmp);
                    }
                }
            }
            oMatrix.setValue(0, 3, 27);
            Assert.IsTrue((int)Math.Round(oMatrix.getValue(0, 3)) == 27, "Incorrect matrix value! Expected 27");
            oMatrix.clear();
            for (iRow = 0; iRow < oMatrix.getNumRows(); iRow++)
            {
                for (iColumn = 0; iColumn < oMatrix.getNumColumns(); iColumn++)
                {
                    Assert.IsTrue((int)Math.Round(oMatrix.getValue(iRow, iColumn)) == 0, "Incorrect matrix value! Expected 0");
                }
            }
            oMatrix.clone();
            oMatrix.Dispose();
        }

        [TestMethod]
        public void TestPLU_Factorization()
        {
            PLU_Factorization oPLU_Factorization = new PLU_Factorization();
            oPLU_Factorization = new PLU_Factorization(); // Check for memory access problems
            oPLU_Factorization.Dispose();
        }

        [TestMethod]
        public void TestCircuitComponent()
        {
            CircuitComponent oCircuitComponent;

            AssertAction.VerifyAssert(() => oCircuitComponent = new CircuitComponent(5, 5, false), "Expected 'Node values must not be the same!' error, did not get it!");
            AssertAction.VerifyAssert(() => oCircuitComponent = new CircuitComponent(0, -6, false), "Expected 'Node values must be greater than or equal to 0!' error, did not get it!");
            AssertAction.VerifyAssert(() => oCircuitComponent = new CircuitComponent(-5, 6, false), "Expected 'Node values must be greater than or equal to 0!' error, did not get it!");

            oCircuitComponent = new CircuitComponent(0, 5, false);
            Assert.IsTrue(oCircuitComponent.getIsGround() == false, "Incorrect value! Expected false");
            Assert.IsTrue(oCircuitComponent.getNodeS() == 0, "Incorrect value! Expected 0");
            Assert.IsTrue(oCircuitComponent.getNodeD() == 5, "Incorrect value! Expected 5");
            Assert.IsTrue(oCircuitComponent.getCurrent() == 0, "Incorrect value! Expected 0");

            oCircuitComponent = new CircuitComponent(2, 7, true); // Check for memory access problems
        }

        [TestMethod]
        public void TestCapacitor()
        {
            Capacitor oCapacitor = new Capacitor(1, 2, 7);
            oCapacitor = new Capacitor(0, 2, 4);  // Check for memory access problems
            AssertAction.VerifyAssert(() => oCapacitor = new Capacitor(1, 2, -7), "Expected 'Capacitance value must be greater than 0!' error, did not get it!");
        }

        [TestMethod]
        public void TestInductor()
        {
            Inductor oInductor = new Inductor(1, 2, 7);
            oInductor = new Inductor(0, 2, 4);  // Check for memory access problems
            AssertAction.VerifyAssert(() => oInductor = new Inductor(1, 2, -7), "Expected 'Inductance value must be greater than 0!' error, did not get it!");
        }

        [TestMethod]
        public void TestResistor()
        {
            Resistor oResistor = new Resistor(1, 2, 10);
            oResistor = new Resistor(0, 2, 4);  // Check for memory access problems
            AssertAction.VerifyAssert(() => new Resistor(1, 2, -10), "Expected 'Resistance value must be greater than 0!' error, did not get it!");
        }

        [TestMethod]
        public void TestGroundedVoltageSource()
        {
            GroundedVoltageSource oGroundedVoltageSource = new GroundedVoltageSource(1, 2, 7, 10);
            oGroundedVoltageSource = new GroundedVoltageSource(0, 2, 4, 4);  // Check for memory access problems
            AssertAction.VerifyAssert(() => oGroundedVoltageSource = new GroundedVoltageSource(1, 2, -7, 10), "Expected 'Voltage value must be greater than 0!' error, did not get it!");
            AssertAction.VerifyAssert(() => oGroundedVoltageSource = new GroundedVoltageSource(1, 2, 7, -10), "Expected 'Resistance value must be greater than 0!' error, did not get it!");
        }

        [TestMethod]
        public void TestLinearCircuit()
        {
            bool bDone = false;
            int iSteps = 0;
            LinearCircuit oLinearCircuit = new LinearCircuit(4);
            oLinearCircuit.addGroundedVoltageSource(1, 2, 30, 10);
            oLinearCircuit.addCapacitor(1, 0, 10);
            oLinearCircuit.addResistor(3, 2, 10);
            AssertAction.VerifyAssert(() => oLinearCircuit.addGroundedVoltageSource(1, 2, 30, 10), "Expected 'Circuit already has a ground, cannot add another one!' error, did not get it!");
            oLinearCircuit.addInductor(4, 2, 10);
            AssertAction.VerifyAssert(() => oLinearCircuit.addInductor(5, 2, 10), "Expected 'Circuit is already full of components!' error, did not get it!");
            oLinearCircuit.setStopTime(10);
            AssertAction.VerifyAssert(() => oLinearCircuit.setStopTime(0), "Expected 'Stop time must be greater than 0!' error, did not get it!");
            oLinearCircuit.setTimeStep(1);
            AssertAction.VerifyAssert(() => oLinearCircuit.setTimeStep(0), "Expected 'Time step must be greater than 0!' error, did not get it!");
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getTime()) == 0, "Incorrect time! Expected 0");
            AssertAction.VerifyAssert(() => oLinearCircuit.step(), "Expected 'Circuit has not been initalized!' error, did not get it!");
            AssertAction.VerifyAssert(() => oLinearCircuit.getCurrent(0), "Expected 'Cannot read currents from a circuit that has not been simulated!' error, did not get it!");
            AssertAction.VerifyAssert(() => oLinearCircuit.getVoltage(2), "Expected 'Cannot read voltages from a circuit that has not been simulated!' error, did not get it!");
            oLinearCircuit.initalize();
            bDone = oLinearCircuit.step();
            iSteps++;
            AssertAction.VerifyAssert(() => oLinearCircuit.getCurrent(8), "Expected 'Requested component does not exist!' error, did not get it!");
            AssertAction.VerifyAssert(() => oLinearCircuit.getVoltage(-6), "Expected 'Node values must be greater than or equal to 0!' error, did not get it!");
            AssertAction.VerifyAssert(() => oLinearCircuit.getVoltage(7), "Expected 'Requested node does not exist!' error, did not get it!");
            Assert.IsTrue(bDone == false, "Simulation is not supposed to be done on the first step!");
            do
            {
                bDone = oLinearCircuit.step();
                iSteps++;
            }
            while (bDone == false && iSteps < 20);
            Assert.IsTrue(iSteps == 10, "Simulation did not finish in the correct number of time steps!");

            AssertAction.VerifyAssert(() => new LinearCircuit(-5), "Expected 'Circuit must have a positive and non-zero number of components!' error, did not get it!");
            AssertAction.VerifyAssert(() => new LinearCircuit(0), "Expected 'Circuit must have a positive and non-zero number of components!' error, did not get it!");
            oLinearCircuit = new LinearCircuit(4);
            oLinearCircuit.setStopTime(10);
            oLinearCircuit.setTimeStep(1);
            AssertAction.VerifyAssert(() => oLinearCircuit.initalize(), "Expected 'There are no components in the circuit!' error, did not get it!");
            oLinearCircuit.addResistor(0, 1, 10);
            AssertAction.VerifyAssert(() => oLinearCircuit.initalize(), "Expected 'There is no ground in the circuit!' error, did not get it!");
            oLinearCircuit.addGroundedVoltageSource(0, 2, 10, 10);
            oLinearCircuit.setTimeStep(11);
            AssertAction.VerifyAssert(() => oLinearCircuit.initalize(), "Expected 'Stop time cannot be smaller than time step!' error, did not get it!");
            oLinearCircuit.setTimeStep(1);
            oLinearCircuit.addResistor(0, 6, 10);
            AssertAction.VerifyAssert(() => oLinearCircuit.initalize(), "Expected 'Circuit nodes are not condensed into the smallest matrix possible!' error, did not get it!");

            oLinearCircuit.Dispose();
        }
    }

    #endregion

    #region Simulation Integration Test

    [TestClass]
    public class SimulationIntegrationTests
    {
        [TestMethod]
        public void SimulationIntegrationTestR()
        {
            bool bDone;
            int iSteps = 0;
            LinearCircuit oLinearCircuit = new LinearCircuit(3);

            oLinearCircuit.addGroundedVoltageSource(2, 1, 30, 10); // Node 2 is ground
            oLinearCircuit.addResistor(1, 0, 10);
            oLinearCircuit.addResistor(0, 2, 10);
            oLinearCircuit.setStopTime(10);
            oLinearCircuit.setTimeStep(1);
            oLinearCircuit.initalize();

            do
            {
                bDone = oLinearCircuit.step();
                iSteps++;
            }
            while (bDone == false && iSteps < 20);

            Assert.IsTrue(iSteps == 10, "Simulation did not finish in the correct number of time steps!");
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getTime()) == 10, "Incorrect time! Expected 10");
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(0)) == 10, "Incorrect voltage at node 0! Expected 10");
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(1)) == 20, "Incorrect voltage at node 1! Expected 20");
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(2)) == 0, "Incorrect voltage at node 2! Expected 0");
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(0)) == 1, "Incorrect current at component 0! Expected 1");
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(1)) == 1, "Incorrect current at component 1! Expected 1");
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(2)) == 1, "Incorrect current at component 2! Expected 1");

            oLinearCircuit.Dispose();
        }

        [TestMethod]
        public void SimulationIntegrationTestRC()
        {
            bool bDone;
            int iSteps = 0;
            LinearCircuit oLinearCircuit = new LinearCircuit(3);

            oLinearCircuit.addGroundedVoltageSource(2, 1, 30, 10); // Node 2 is ground
            oLinearCircuit.addResistor(1, 0, 10);
            oLinearCircuit.addCapacitor(0, 2, 0.2);
            oLinearCircuit.setStopTime(10);
            oLinearCircuit.setTimeStep(1);
            oLinearCircuit.initalize();

            do
            {
                bDone = oLinearCircuit.step();
                iSteps++;
            }
            while (bDone == false);

            Assert.IsTrue(iSteps == 10, "Simulation did not finish in the correct number of time steps!");
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getTime()) == 10, "Incorrect time! Expected 10");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(0)) == 10, "Incorrect voltage at node 0! Expected 10");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(1)) == 20, "Incorrect voltage at node 1! Expected 20");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(2)) == 0, "Incorrect voltage at node 2! Expected 0");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(0)) == 1, "Incorrect current at component 0! Expected 1");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(1)) == 1, "Incorrect current at component 1! Expected 1");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(2)) == 1, "Incorrect current at component 2! Expected 1");

            oLinearCircuit.Dispose();
        }

        [TestMethod]
        public void SimulationIntegrationTestRL()
        {
            bool bDone;
            int iSteps = 0;
            LinearCircuit oLinearCircuit = new LinearCircuit(3);

            oLinearCircuit.addGroundedVoltageSource(2, 1, 30, 10); // Node 2 is ground
            oLinearCircuit.addResistor(1, 0, 10);
            oLinearCircuit.addInductor(0, 2, 0.2);
            oLinearCircuit.setStopTime(10);
            oLinearCircuit.setTimeStep(1);
            oLinearCircuit.initalize();

            do
            {
                bDone = oLinearCircuit.step();
                iSteps++;
            }
            while (bDone == false);

            Assert.IsTrue(iSteps == 10, "Simulation did not finish in the correct number of time steps!");
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getTime()) == 10, "Incorrect time! Expected 10");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(0)) == 10, "Incorrect voltage at node 0! Expected 10");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(1)) == 20, "Incorrect voltage at node 1! Expected 20");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(2)) == 0, "Incorrect voltage at node 2! Expected 0");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(0)) == 1, "Incorrect current at component 0! Expected 1");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(1)) == 1, "Incorrect current at component 1! Expected 1");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(2)) == 1, "Incorrect current at component 2! Expected 1");

            oLinearCircuit.Dispose();
        }
    }

    #endregion
}