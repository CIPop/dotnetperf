// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using Perf;
using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace HttpClientPerfUWP
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
        }

        private async void btnRun_Click(object sender, RoutedEventArgs e)
        {
            string originalText = (string)_btnRun.Content;
            _btnRun.IsEnabled = false;
            _btnRun.Content = "Test running...";

            Stopwatch wallTime = new Stopwatch();

            try
            {
                Uri server = new Uri(_txtServer.Text);
                int iterations = int.Parse(_txtIterations.Text);
                int instances = int.Parse(_txtClientInstances.Text);
                int requestSizeCharacters = int.Parse(_txtPostCharacters.Text);
                bool sync = _cmbSync.IsSelected;

                await Task.Run(() =>
                {
                    var folder = KnownFolders.SavedPictures;

                    ResultWriter writer = new ResultWriterFile(Path.Combine(folder.Path, "out.csv"));

                    Func<int, PerfTest> factory = (index) =>
                    {
                        return new UWPHttpClientPerfTestAsync(writer, index, wallTime, server, iterations, requestSizeCharacters);
                    };

                    wallTime.Start();
                    if (sync || (instances == 1))
                    {
                        PerfTestExecution.RunSquential(writer, instances, factory).GetAwaiter().GetResult();
                    }
                    else
                    {
                        PerfTestExecution.RunParallel(writer, instances, factory).GetAwaiter().GetResult();
                    }

                    wallTime.Stop();
                });
            }
            catch (Exception ex)
            {
                var errDialog = new MessageDialog(ex.ToString(), "Error");
                await errDialog.ShowAsync();
            }

            var d = new MessageDialog(wallTime.Elapsed.ToString(), "Test complete");
            await d.ShowAsync();

            _btnRun.IsEnabled = true;
            _btnRun.Content = originalText;
        }
    }
}
