using ElectricalCircuitSimulator;
using System;
using System.Net.Quic;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace UnitTests
{
    public class STATestMethod : TestMethodAttribute
    {
        public override TestResult[] Execute(ITestMethod testMethod)
        {
            if (Thread.CurrentThread.GetApartmentState() == ApartmentState.STA)
                return Invoke(testMethod);

            TestResult[] oResult = null;
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
            MainWindow oMainWindow = null;
            
            RoutedEventArgs oRoutedEventArgs = new RoutedEventArgs();
            TextChangedEventArgs oTextChangedEventArgs = new TextChangedEventArgs(TextBox.TextChangedEvent, UndoAction.None);
            MouseButtonEventArgsTest oMouseButtonEventArgsTest = new MouseButtonEventArgsTest(Mouse.PrimaryDevice, MouseButton.Left, MouseButtonState.Pressed);
           
            IndexTextBlock oIndexTextBlock = null;
            TextBox oTextBox = new TextBox();
            Button oButton = new Button();

            const int iFilesLoaded = 5;
            int iIndex = iFilesLoaded;
            bool bSuccess = false;

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
}