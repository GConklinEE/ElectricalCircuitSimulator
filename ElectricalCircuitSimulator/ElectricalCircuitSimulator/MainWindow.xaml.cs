using OxyPlot.Series;
using OxyPlot.Wpf;
using OxyPlot;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.IO;
using System.Collections.ObjectModel;

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

        public MainWindow(bool bTestMode)
        {
            m_bTestMode = bTestMode;
            Setup();
        }

        #endregion

        #region Functions

        private void Setup()
        {
            InitializeComponent();

            m_oImages = new Collection<Image>();
            LoadCircuits();

            // Create and configure the PlotModel
            m_oPlotModel1 = new PlotModel { Title = "Dynamic Graph 1" };
            m_oPlotModel2 = new PlotModel { Title = "Dynamic Graph 2" };
            m_oLineSeries1 = new LineSeries { Title = "Data Series 1" };
            m_oLineSeries2 = new LineSeries { Title = "Data Series 2" };
            m_oPlotModel1.Series.Add(m_oLineSeries1);
            m_oPlotModel2.Series.Add(m_oLineSeries2);

            // Bind the PlotModel to the PlotView
            xPlotView1.Model = m_oPlotModel1;
            xPlotView2.Model = m_oPlotModel2;

            // Set up a timer to update the graph dynamically
            m_oTimer = new DispatcherTimer
            {
                Interval = TimeSpan.FromSeconds(1)
            };
            m_oTimer.Tick += Timer_Tick;
        }

        private void LoadCircuits()
        {
            // Go to the folder where circuit files and images are contained, and load them all
            Image oBlankImage = new Image();
            string[] sfiles = new string[0];
            string sDirectoryPath = @"../../../../../Circuits";
            string sFileType = "*.txt";

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
                oBlankImage.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath("../../../../../Circuits/Blank.png")));
            }
            catch
            {
                if (m_bTestMode == false)
                {
                    MessageBox.Show("No blank circuit image found!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                UI_Error = true;
            }

            for (int i = 0; i < sfiles.Length; i++)
            {
                Image oImage = new Image();
                try
                {
                    oImage.Source = new BitmapImage(new Uri(System.IO.Path.GetFullPath(System.IO.Path.ChangeExtension(sfiles[i], ".png"))));
                }
                catch
                {
                    oImage = oBlankImage;
                    UI_Error = true;
                }

                oImage.Width = 490;
                oImage.Height = 340;
                m_oImages.Add(oImage);

                IndexTextBlock oTextBlock = new IndexTextBlock(i);
                oTextBlock.Text = "Circuit " + (i + 1).ToString();
                oTextBlock.FontSize = 15;
                oTextBlock.Margin = new Thickness(4, 5, 5, 0);
                oTextBlock.Padding = new Thickness(10, 10, 10, 10);
                oTextBlock.MouseLeftButtonDown += TextBox_MouseLeftButtonDown;
                xCircuitList.Children.Add(oTextBlock);

                if (i == 0)
                {
                    oTextBlock.Background = new SolidColorBrush(Colors.DarkRed);
                    oTextBlock.Foreground = new SolidColorBrush(Colors.White);
                    m_oLastTextBlock = oTextBlock;
                    xCircuitImage.Content = oImage;
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
            Double dNewX1 = m_oLineSeries1.Points.Count > 0 ? m_oLineSeries1.Points[m_oLineSeries1.Points.Count - 1].X + 1 : 0;
            Double dNewX2 = m_oLineSeries2.Points.Count > 0 ? m_oLineSeries2.Points[m_oLineSeries2.Points.Count - 1].X + 1 : 0;
            Double dNewY = new Random().NextDouble() * 20;
            m_oLineSeries1.Points.Add(new DataPoint(dNewX1, dNewY));
            m_oLineSeries2.Points.Add(new DataPoint(dNewX2, dNewY));

            if (m_oLineSeries1.Points.Count > 100) // Keep only last 100 points
            {
                m_oLineSeries1.Points.RemoveAt(0);
            }
            if (m_oLineSeries2.Points.Count > 100) // Keep only last 100 points
            {
                m_oLineSeries2.Points.RemoveAt(0);
            }

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

        public void StepNumTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            int result;
            if (int.TryParse(xStepNumTextBox.Text, out result))
            {
                if (result <= 0)
                {
                    xStepNumTextBox.Background = new SolidColorBrush(Colors.Red);
                }
                else
                {
                    xStepNumTextBox.Background = new SolidColorBrush(Colors.White);
                    m_iStepNum = result;
                }
            }
            else
            {
                xStepNumTextBox.Background = new SolidColorBrush(Colors.Red);
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
            if (m_oTimer.IsEnabled)
            {
                xSimulateButton.Content = "Simulate Circuit";
                m_oTimer.Stop();
            }
            else
            {
                xSimulateButton.Content = "Halt Simulation";
                m_oTimer.Start();
            }
        }

        #endregion

        #region Data Members

        public bool m_bTestMode;
        private double m_dStepSize;
        private int m_iStepNum;
        private LineSeries m_oLineSeries1;
        private LineSeries m_oLineSeries2;
        private PlotModel m_oPlotModel1;
        private PlotModel m_oPlotModel2;
        private DispatcherTimer m_oTimer;
        private IndexTextBlock m_oLastTextBlock;
        private Collection<Image> m_oImages;

        #endregion

        #region Properties

        public bool UI_Error { get; private set; }

        #endregion 
    }
}

#region Classes

public class IndexTextBlock : TextBlock
{
    public IndexTextBlock(int iIndex) : base()
    {
        m_iIndex = iIndex;
    }

    public int Index
    {
        get => m_iIndex;
        set => m_iIndex = value;
    }

    private int m_iIndex;
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

    public MouseButtonState LeftButton { get; set; }
    public MouseButtonState RightButton { get; set; }
    public MouseButtonState MiddleButton { get; set; }
}

#endregion
