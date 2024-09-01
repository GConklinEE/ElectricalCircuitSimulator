using ElectricalCircuitSimulator;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using SimulationEngineWrapper;

namespace UnitTests
{
    #region Test UI

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
            bool bSuccess = false;

            Assert.IsTrue(oIndexTextBlock.Index == 0, "Index property returned unexpected value after being configured with blank constructor!");
            try
            {
                oIndexTextBlock = new IndexTextBlock(iIndex);
            }
            catch
            {
                Assert.Fail("Failed to instatiate IndexTextBlock!");
            }
            Assert.IsTrue(oIndexTextBlock.Index == iIndex, "Index property returned unexpected value after being configured with constructor!");
            iIndex++;
            oIndexTextBlock.Index = iIndex;
            Assert.IsTrue(oIndexTextBlock.Index == iIndex, "Index property returned unexpected value after being configured with property set method!");

            try
            {
                oMainWindow = new MainWindow(true);
            }
            catch
            {
                Assert.Fail("Failed to instatiate MainWindow!");
            }
            finally
            {
                if ((oMainWindow != null) && (oMainWindow.UI_Error))
                {
                    Assert.Fail("UI error in MainWindow!");
                }
            }

            try
            {
                oIndexTextBlock.Index = 2;
                oMainWindow.TextBox_MouseLeftButtonDown(oIndexTextBlock, oMouseButtonEventArgsTest);
            }
            catch
            {
                Assert.Fail("Failed to run TextBox_MouseLeftButtonDown!");
            }

            try
            {
                bSuccess = false;
                oIndexTextBlock.Index = iFilesLoaded;
                oMainWindow.TextBox_MouseLeftButtonDown(oIndexTextBlock, oMouseButtonEventArgsTest);
                bSuccess = true;
            }
            catch
            {}
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Loaded more files than expected!");
                }
            }

            try
            {
                bSuccess = false;
                oIndexTextBlock.Index = iFilesLoaded - 1;
                oMainWindow.TextBox_MouseLeftButtonDown(oIndexTextBlock, oMouseButtonEventArgsTest);
                bSuccess = true;
            }
            catch
            {
                Assert.Fail("Loaded less files than expected!");
            }

            try
            {
                oTextBox.Text = "5";
                oMainWindow.StepSizeTextBox_TextChanged(oTextBox, oTextChangedEventArgs);
                oTextBox.Text = "-1";
                oMainWindow.StepSizeTextBox_TextChanged(oTextBox, oTextChangedEventArgs);
                oTextBox.Text = "A";
                oMainWindow.StepSizeTextBox_TextChanged(oTextBox, oTextChangedEventArgs);
            }
            catch
            {
                Assert.Fail("Failed to run StepSizeTextBox_TextChanged!");
            }

            try
            {
                oTextBox.Text = "5";
                oMainWindow.StepNumTextBox_TextChanged(oTextBox, oTextChangedEventArgs);
                oTextBox.Text = "-1";
                oMainWindow.StepNumTextBox_TextChanged(oTextBox, oTextChangedEventArgs);
                oTextBox.Text = "A";
                oMainWindow.StepNumTextBox_TextChanged(oTextBox, oTextChangedEventArgs);
            }
            catch
            {
                Assert.Fail("Failed to run StepNumTextBox_TextChanged!");
            }

            try
            {
                oMainWindow.Screen_MouseDown(oIndexTextBlock, oMouseButtonEventArgsTest);
            }
            catch
            {
                Assert.Fail("Failed to run Screen_MouseDown!");
            }

            try
            {
                bSuccess = false;
                oMainWindow.CloseButton_Click(oButton, oRoutedEventArgs);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected running CloseButton_Click to fail, it did not!");
                }
            }

            try
            {
                oMainWindow.MinimizeButton_Click(oButton, oRoutedEventArgs);
            }
            catch
            {
                Assert.Fail("Failed to run MinimizeButton_Click!");
            }

            try
            {
                // Event toggles states, so call it twice
                oMainWindow.SimulateButton_Click(oButton, oRoutedEventArgs);
                oMainWindow.SimulateButton_Click(oButton, oRoutedEventArgs);
            }
            catch
            {
                Assert.Fail("Failed to run SimulateButton_Click!");
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
            bool bSuccess = false;

            Matrix oMatrix = new Matrix();
            oMatrix = new Matrix(2, 6);
            try
            {
                bSuccess = false;
                oMatrix = new Matrix(0, 5);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oMatrix = new Matrix(6, 0);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oMatrix = new Matrix(-6, 6);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oMatrix = new Matrix(6, -6);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            oMatrix = new Matrix(5, 5);
            Assert.IsTrue(oMatrix.getNumRows() == 5, "Incorrect matrix row count! Expected 5");
            Assert.IsTrue(oMatrix.getNumColumns() == 5, "Incorrect matrix column count! Expected 5");
            oMatrix.printMatrix();
            try
            {
                bSuccess = false;
                oMatrix.setValue(3, 5, 11);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oMatrix.setValue(5, 3, 11);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oMatrix.getValue(3, 5);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oMatrix.getValue(5, 3);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
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
        }

        [TestMethod]
        public void TestCircuitComponent()
        {
            bool bSuccess = false;
            CircuitComponent oCircuitComponent;
            try
            {
                bSuccess = false;
                oCircuitComponent = new CircuitComponent(5, 5, false);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oCircuitComponent = new CircuitComponent(0, -6, false);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oCircuitComponent = new CircuitComponent(-5, 6, false);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            oCircuitComponent = new CircuitComponent(0, 5, false);
            Assert.IsTrue(oCircuitComponent.getIsGround() == false, "Incorrect value! Expected false");
            Assert.IsTrue(oCircuitComponent.getNodeS() == 0, "Incorrect value! Expected 0");
            Assert.IsTrue(oCircuitComponent.getNodeD() == 5, "Incorrect value! Expected 5");
            Assert.IsTrue(oCircuitComponent.getCurrent() == 0, "Incorrect value! Expected 0");
        }

        [TestMethod]
        public void TestCapacitor()
        {
            bool bSuccess = false;

            Capacitor oCapacitor = new Capacitor(1, 2, 7);
            try
            {
                bSuccess = false;
                oCapacitor = new Capacitor(1, 2, -7);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
        }

        [TestMethod]
        public void TestInductor()
        {
            bool bSuccess = false;

            Inductor oInductor = new Inductor(1, 2, 7);
            try
            {
                bSuccess = false;
                oInductor = new Inductor(1, 2, -7);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
        }

        [TestMethod]
        public void TestResistor()
        {
            bool bSuccess = false;

            Resistor oResistor = new Resistor(1, 2, 10);
            try
            {
                bSuccess = false;
                oResistor = new Resistor(1, 2, -10);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
        }

        [TestMethod]
        public void TestGroundedVoltageSource()
        {
            bool bSuccess = false;

            GroundedVoltageSource oGroundedVoltageSource = new GroundedVoltageSource(1, 2, 7, 10);
            try
            {
                bSuccess = false;
                oGroundedVoltageSource = new GroundedVoltageSource(1, 2, -7, 10);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oGroundedVoltageSource = new GroundedVoltageSource(1, 2, 7, -10);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
        }

        [TestMethod]
        public void TestLinearCircuit()
        {
            bool bDone = false;
            bool bSuccess = false;
            LinearCircuit oLinearCircuit = new LinearCircuit(4);

            oLinearCircuit.addGroundedVoltageSource(1, 2, 30, 10);
            oLinearCircuit.addCapacitor(1, 0, 10);
            oLinearCircuit.addResistor(3, 2, 10);
            try
            {
                bSuccess = false;
                oLinearCircuit.addGroundedVoltageSource(1, 2, 30, 10);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            oLinearCircuit.addInductor(5, 2, 10);
            try
            {
                bSuccess = false;
                oLinearCircuit.addInductor(5, 2, 10);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            oLinearCircuit.setStopTime(10);
            try
            {
                bSuccess = false;
                oLinearCircuit.setStopTime(0);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            oLinearCircuit.setTimeStep(1);
            try
            {
                bSuccess = false;
                oLinearCircuit.setTimeStep(0);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            Assert.IsTrue((int)Math.Round(oLinearCircuit.getTime()) == 0, "Incorrect time! Expected 0");
            try
            {
                bSuccess = false;
                oLinearCircuit.getVoltage(2);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oLinearCircuit.getVoltage(7);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oLinearCircuit.getCurrent(0);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            try
            {
                bSuccess = false;
                oLinearCircuit.getCurrent(8);
                bSuccess = true;
            }
            catch
            { }
            finally
            {
                if (bSuccess == true)
                {
                    Assert.Fail("Expected error, did not get it!");
                }
            }
            //oLinearCircuit.initalize();
            //oLinearCircuit.step();

            //oLinearCircuit.Dispose();
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
            LinearCircuit oLinearCircuit = new LinearCircuit(3);

            oLinearCircuit.addGroundedVoltageSource(1, 2, 30, 10);
            oLinearCircuit.addResistor(1, 0, 10);
            oLinearCircuit.addResistor(0, 2, 10);

            oLinearCircuit.setStopTime(10);
            oLinearCircuit.setTimeStep(1);

            //oLinearCircuit.initalize();
            //do
            //{
            //    bDone = oLinearCircuit.step();
            //}
            //while (bDone == false);

            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getTime()) == 10, "Incorrect time! Expected 10");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(0)) == 10, "Incorrect voltage at node 0! Expected 10");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(1)) == 20, "Incorrect voltage at node 1! Expected 20");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getVoltage(2)) == 0, "Incorrect voltage at node 2! Expected 0");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(0)) == 1, "Incorrect current at component 0! Expected 1");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(1)) == 1, "Incorrect current at component 1! Expected 1");
            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getCurrent(2)) == 1, "Incorrect current at component 2! Expected 1");

            //oLinearCircuit.Dispose();
        }

        [TestMethod]
        public void SimulationIntegrationTestRC()
        {
            LinearCircuit oLinearCircuit = new LinearCircuit(3);

            oLinearCircuit.addGroundedVoltageSource(1, 2, 30, 10);
            oLinearCircuit.addResistor(1, 0, 10);
            oLinearCircuit.addCapacitor(0, 2, 0.0001);

            oLinearCircuit.setStopTime(10);
            oLinearCircuit.setTimeStep(1);

            //oLinearCircuit.initalize();
            //do
            //{
            //    bDone = oLinearCircuit.step();
            //}
            //while (bDone == false);

            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getTime()) == 10, "Incorrect time! Expected 10");

            //oLinearCircuit.Dispose();
        }

        [TestMethod]
        public void SimulationIntegrationTestRL()
        {
            LinearCircuit oLinearCircuit = new LinearCircuit(3);

            oLinearCircuit.addGroundedVoltageSource(1, 2, 30, 10);
            oLinearCircuit.addResistor(1, 0, 10);
            oLinearCircuit.addInductor(0, 2, 0.0001);

            oLinearCircuit.setStopTime(10);
            oLinearCircuit.setTimeStep(1);

            //oLinearCircuit.initalize();
            //do
            //{
            //    bDone = oLinearCircuit.step();
            //}
            //while (bDone == false);

            //Assert.IsTrue((int)Math.Round(oLinearCircuit.getTime()) == 10, "Incorrect time! Expected 10");

            //oLinearCircuit.Dispose();
        }
    }

    #endregion
}