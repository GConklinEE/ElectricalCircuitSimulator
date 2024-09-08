using OxyPlot.Series;
using OxyPlot;
using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using System.IO;
using System.Collections.ObjectModel;
using SimulationEngineWrapper;

namespace ElectricalCircuitSimulator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        #region Constructors

        public MainWindow()
        {
            m_bTestMode = false;
            Setup();
        }

        public MainWindow(bool bTestMode) // Set to true if running this class in a unit testing framework
        {
            m_bTestMode = bTestMode;
            Setup();
        }

        public MainWindow(int iValue) {} // Blank constructor for testing

        #endregion

        #region Functions

        private void Setup()
        {
            InitializeComponent();

            m_oImages = new Collection<Image>();
            LoadCircuits();

            // Create and configure the PlotModel
            m_oPlotModel1 = new PlotModel { Title = "V(t) Scope" };
            m_oPlotModel2 = new PlotModel { Title = "I(t) Scope" };
            m_oLineSeriesV = new LineSeries { Title = "V(t) Scope" };
            m_oLineSeriesI = new LineSeries { Title = "I(t) Scope" };
            m_oPlotModel1.Series.Add(m_oLineSeriesV);
            m_oPlotModel2.Series.Add(m_oLineSeriesI);

            // Bind the PlotModel to the PlotView
            xPlotView1.Model = m_oPlotModel1;
            xPlotView2.Model = m_oPlotModel2;
        }

        private void LoadCircuits()
        {
            // Go to the folder where circuit files and images are contained, and load them all
            Image oBlankImage = new Image();
            Collection<Collection<string>> oFileTextLines = new Collection<Collection<string>>();
            Collection<string> oTextLines;
            StreamReader oStreamReader;
            LinearCircuit oLinearCircuit;
            string[] sfiles = new string[0];
            string[] sComponentParams;
            string sDirectoryPath = @"../../../../../../Circuits";
            string sFileType = "*.txt";
            string sFileTextLine;
            string sName;
            double[] dParamValue = new double[2];
            bool bFirstListSelectionConfigured = false;
            int iFile;
            int iLineIndex;
            int iLoadedFile = 0;
            int iComponent;
            int iComponents;
            int[] iParamValue = new int[2];

            m_oSimulations = new Collection<LinearCircuit>();
            m_oVoltageScopeNodes = new Collection<int>();
            m_oCurrentScopeComponents = new Collection<int>();
            m_oTimeSteps = new Collection<double>();
            m_oStopTimes = new Collection<double>();

            try
            {
                sfiles = Directory.GetFiles(sDirectoryPath, sFileType, SearchOption.AllDirectories);
            }
            catch
            {
                if (m_bTestMode == false) 
                {
                    MessageBox.Show("No circuit files found!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                UI_Error = true;
            }

            try
            {
                oBlankImage.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("../../../../../../Circuits/Blank.png")));
            }
            catch
            {
                if (m_bTestMode == false)
                {
                    MessageBox.Show("No blank circuit image found!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                UI_Error = true;
            }

            for (iFile = 0; iFile < sfiles.Length; iFile++)
            {
                // Load circuit file data
                try
                {
                    oTextLines = new Collection<string>();
                    oFileTextLines.Add(oTextLines);
                    oStreamReader = new StreamReader(sfiles[iFile]);
                    do
                    {
                        sFileTextLine = oStreamReader.ReadLine();
                        if (sFileTextLine != null) {
                            oTextLines.Add(sFileTextLine);
                        }
                    } while (sFileTextLine != null);
                    oStreamReader.Close();

                    // Build the simulations

                    // Components
                    iLineIndex = FileConstants.HEADER_LINES;
                    iComponents = oTextLines.Count() - FileConstants.ALL_EXCEPT_COMPONENTS;
                    oLinearCircuit = new LinearCircuit(iComponents);
                    for (iComponent = (iLineIndex); iComponent < (iComponents + iLineIndex); iComponent++)
                    {
                        sName = oTextLines[iComponent].Split("(")[0];
                        sComponentParams = oTextLines[iComponent].Split("(")[1].Split(")")[0].Split(",");
                        switch (sName)
                        {
                            case "Resistor":
                                oLinearCircuit.addResistor(int.Parse(sComponentParams[0]), int.Parse(sComponentParams[1]), double.Parse(sComponentParams[2]));
                                break;
                            case "Capacitor":
                                oLinearCircuit.addCapacitor(int.Parse(sComponentParams[0]), int.Parse(sComponentParams[1]), double.Parse(sComponentParams[2]));
                                break;
                            case "Inductor":
                                oLinearCircuit.addInductor(int.Parse(sComponentParams[0]), int.Parse(sComponentParams[1]), double.Parse(sComponentParams[2]));
                                break;
                            case "GroundedVoltageSource":
                                oLinearCircuit.addGroundedVoltageSource(int.Parse(sComponentParams[0]), int.Parse(sComponentParams[1]), double.Parse(sComponentParams[2]), double.Parse(sComponentParams[3]));
                                break;
                            default:
                                throw new Exception();
                        }
                    }

                    // Scopes
                    iLineIndex += iComponents;
                    sName = oTextLines[iLineIndex].Split("(")[0];
                    iParamValue[0] = int.Parse(oTextLines[iLineIndex].Split("(")[1].Split(")")[0]);
                    if (sName != "ScopeV")
                    {
                        throw new Exception();
                    }

                    iLineIndex += FileConstants.SCOPE_V_LINES;
                    sName = oTextLines[iLineIndex].Split("(")[0];
                    iParamValue[1] = int.Parse(oTextLines[iLineIndex].Split("(")[1].Split(")")[0]);
                    if (sName != "ScopeI")
                    {
                        throw new Exception();
                    }

                    // Time params
                    iLineIndex += FileConstants.SCOPE_I_LINES;
                    sName = oTextLines[iLineIndex].Split("(")[0];
                    dParamValue[0] = double.Parse(oTextLines[iLineIndex].Split("(")[1].Split(")")[0]);
                    if (sName != "TimeStep")
                    {
                        throw new Exception();
                    }

                    iLineIndex += FileConstants.TIME_STEP_LINES;
                    sName = oTextLines[iLineIndex].Split("(")[0];
                    dParamValue[1] = double.Parse(oTextLines[iLineIndex].Split("(")[1].Split(")")[0]);
                    if (sName != "StopTime")
                    {
                        throw new Exception();
                    }

                    // Add everything to collections at the end, so errors don't result in mismatching collection indexes
                    m_oVoltageScopeNodes.Add(iParamValue[0]);
                    m_oCurrentScopeComponents.Add(iParamValue[1]);
                    m_oTimeSteps.Add(dParamValue[0]);
                    m_oStopTimes.Add(dParamValue[1]);
                    m_oSimulations.Add(oLinearCircuit);
                }
                catch
                {
                    UI_Error = true;
                    continue; // Move on if we can't open this file
                }
                iLoadedFile++;

                // Setup circuit images
                Image oImage = new Image();
                try
                {
                    oImage.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath(System.IO.Path.ChangeExtension(sfiles[iFile], ".png"))));
                }
                catch
                {
                    oImage = oBlankImage;
                    UI_Error = true;
                }

                oImage.Width = 490;
                oImage.Height = 340;
                m_oImages.Add(oImage);

                // Setup selection interface
                IndexTextBlock oTextBlock = new IndexTextBlock(iLoadedFile - 1);
                oTextBlock.Text = oFileTextLines[iFile][FileConstants.START_FILE];
                oTextBlock.FontSize = 15;
                oTextBlock.Margin = new Thickness(4, 5, 5, 0);
                oTextBlock.Padding = new Thickness(10, 10, 10, 10);
                oTextBlock.MouseLeftButtonDown += TextBox_MouseLeftButtonDown;
                xCircuitList.Children.Add(oTextBlock);

                if (!bFirstListSelectionConfigured)
                {
                    oTextBlock.Background = new SolidColorBrush(Colors.DarkRed);
                    oTextBlock.Foreground = new SolidColorBrush(Colors.White);
                    m_oLastTextBlock = oTextBlock;
                    xCircuitImage.Content = oImage;
                    m_dStepSize = m_oTimeSteps[iLoadedFile - 1];
                    m_dStopTime = m_oStopTimes[iLoadedFile - 1];
                    xStepSizeTextBox.Text = "" + m_dStepSize;
                    xStopTimeTextBox.Text = "" + m_dStopTime;
                    bFirstListSelectionConfigured = true;
                }
                else
                {
                    oTextBlock.Background = new SolidColorBrush(Colors.White);
                    oTextBlock.Foreground = new SolidColorBrush(Colors.Black);
                }
            }
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            bool bDone;

            bDone = m_oSimulations[m_oLastTextBlock.Index].step();

            if (bDone == true)
            {
                xSimulateButton.Content = "Simulate Circuit";
                m_oTimer.Stop();
            }

            m_oLineSeriesV.Points.Add(new DataPoint(m_oSimulations[m_oLastTextBlock.Index].getTime(), m_oSimulations[m_oLastTextBlock.Index].getVoltage(m_oVoltageScopeNodes[m_oLastTextBlock.Index])));
            m_oLineSeriesI.Points.Add(new DataPoint(m_oSimulations[m_oLastTextBlock.Index].getTime(), m_oSimulations[m_oLastTextBlock.Index].getCurrent(m_oCurrentScopeComponents[m_oLastTextBlock.Index])));

            // Refresh the plot
            m_oPlotModel1.InvalidatePlot(true);
            m_oPlotModel2.InvalidatePlot(true);
        }

        #endregion

        #region Events

        public void TextBox_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            m_oLastTextBlock.Background = new SolidColorBrush(Colors.White);
            m_oLastTextBlock.Foreground = new SolidColorBrush(Colors.Black);
            m_oLastTextBlock = sender as IndexTextBlock;
            m_oLastTextBlock.Background = new SolidColorBrush(Colors.DarkRed);
            m_oLastTextBlock.Foreground = new SolidColorBrush(Colors.White);
            xCircuitImage.Content = m_oImages.ElementAt(m_oLastTextBlock.Index);
            m_dStepSize = m_oTimeSteps[m_oLastTextBlock.Index];
            m_dStopTime = m_oStopTimes[m_oLastTextBlock.Index];
            xStepSizeTextBox.Text = "" + m_dStepSize;
            xStopTimeTextBox.Text = "" + m_dStopTime;
            if (m_oTimer != null && m_oTimer.IsEnabled)
            {
                xSimulateButton.Content = "Simulate Circuit";
                m_oTimer.Stop();
            }
        }

        public void StepSizeTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            double result;
            if (double.TryParse(xStepSizeTextBox.Text, out result))
            {
                if (result <= 0)
                {
                    xStepSizeTextBox.Background = new SolidColorBrush(Colors.Red);
                }
                else
                {
                    xStepSizeTextBox.Background = new SolidColorBrush(Colors.White);
                    m_dStepSize = result;
                }
            }
            else
            {
                xStepSizeTextBox.Background = new SolidColorBrush(Colors.Red);
            }
        }

        public void StopTimeTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            double result;
            if (double.TryParse(xStopTimeTextBox.Text, out result))
            {
                if (result <= 0)
                {
                    xStopTimeTextBox.Background = new SolidColorBrush(Colors.Red);
                }
                else
                {
                    xStopTimeTextBox.Background = new SolidColorBrush(Colors.White);
                    m_dStopTime = result;
                }
            }
            else
            {
                xStopTimeTextBox.Background = new SolidColorBrush(Colors.Red);
            }
        }

        public void Screen_MouseDown(object sender, MouseButtonEventArgs e)
        {
            MouseButtonState eMouseButtonState;
            if (e is MouseButtonEventArgsTest)
            {
                eMouseButtonState = (e as MouseButtonEventArgsTest).LeftButton;
            }
            else
            {
                eMouseButtonState = e.LeftButton;
            }
            // If the left mouse button is pressed, initiate window drag
            if (eMouseButtonState == MouseButtonState.Pressed)
            {
                this.DragMove();
            }
        }

        public void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        public void MinimizeButton_Click(object sender, RoutedEventArgs e)
        {
            this.WindowState = WindowState.Minimized;
        }

        public void SimulateButton_Click(object sender, RoutedEventArgs e)
        {
            if (m_oTimer != null && m_oTimer.IsEnabled)
            {
                xSimulateButton.Content = "Simulate Circuit";
                m_oTimer.Stop();
            }
            else
            {
                if (m_dStopTime < m_dStepSize)
                {
                    if (m_bTestMode == false)
                    {
                        MessageBox.Show("The stop time cannot be less than the step size!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                }
                else
                {
                    try
                    {
                        // Setup simulation
                        m_oSimulations[m_oLastTextBlock.Index].setStopTime(m_dStopTime);
                        m_oSimulations[m_oLastTextBlock.Index].setTimeStep(m_dStepSize);
                        m_oSimulations[m_oLastTextBlock.Index].initalize();

                        m_oLineSeriesV.Points.Clear();
                        m_oLineSeriesI.Points.Clear();

                        // Set up a timer to update the graph dynamically
                        m_oTimer = new DispatcherTimer
                        {
                            Interval = TimeSpan.FromSeconds(0.001)
                        };
                        m_oTimer.Tick += Timer_Tick;
                        m_oTimer.Start();

                        xSimulateButton.Content = "Halt Simulation";
                    }
                    catch
                    {
                        if (m_bTestMode == false)
                        {
                            MessageBox.Show("Failed to start simulation!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                        }
                    }
                }
            }
        }

        #endregion

        #region Data Members

        public bool m_bTestMode;
        private double m_dStepSize;
        private double m_dStopTime;
        private LineSeries m_oLineSeriesV;
        private LineSeries m_oLineSeriesI;
        private PlotModel m_oPlotModel1;
        private PlotModel m_oPlotModel2;
        private DispatcherTimer m_oTimer;
        private IndexTextBlock m_oLastTextBlock;
        private Collection<Image> m_oImages;
        private Collection<LinearCircuit> m_oSimulations;
        private Collection<int> m_oVoltageScopeNodes;
        private Collection<int> m_oCurrentScopeComponents;
        private Collection<double> m_oTimeSteps;
        private Collection<double> m_oStopTimes;

        #endregion

        #region Properties

        public bool UI_Error { get; private set; }

        #endregion 
    }
}

#region Classes

public static class FileConstants
{
    public const int START_FILE = 0;
    public const int HEADER_LINES = 1;
    public const int SCOPE_V_LINES = 1;
    public const int SCOPE_I_LINES = 1;
    public const int TIME_STEP_LINES = 1;
    public const int STOP_TIME_LINES = 1;
    public const int ALL_EXCEPT_COMPONENTS = HEADER_LINES + SCOPE_V_LINES + SCOPE_I_LINES + TIME_STEP_LINES + STOP_TIME_LINES;
}

public class IndexTextBlock : TextBlock
{
    public IndexTextBlock() : base() { }

    public IndexTextBlock(int iIndex) : base()
    {
        Index = iIndex;
    }

    public int Index { get; set; }
}

public class MouseButtonEventArgsTest : MouseButtonEventArgs
{
    public MouseButtonEventArgsTest(MouseDevice oMouseDevice, MouseButton eButton, MouseButtonState eState)
        : base(oMouseDevice, 0, eButton)
    {
        switch (eButton)
        {
            case MouseButton.Left:
                LeftButton = eState;
                break;
            case MouseButton.Right:
                RightButton = eState;
                break;
            case MouseButton.Middle:
            default:
                MiddleButton = eState;
                break;
        }
    }

    public new MouseButtonState LeftButton { get; set; }
    public new MouseButtonState RightButton { get; set; }
    public new MouseButtonState MiddleButton { get; set; }
}

#endregion
