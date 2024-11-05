using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;
using System.IO;
namespace Installer
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // The path to the key where Windows looks for startup applications
            RegistryKey rkApp = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", true);

            // The value doesn't exist, the application is not set to run at startup
            rkApp.SetValue("PyClientLauncher", Directory.GetCurrentDirectory() + "\\..\\Tray\\WindowsFormsApplication1.exe");
            button2.Enabled = true;

            //Create process
            System.Diagnostics.Process pProcess = new System.Diagnostics.Process();

            //strCommand is path and file name of command to run
            ;
            pProcess.StartInfo.FileName = Directory.GetCurrentDirectory() + "\\..\\Tray\\WindowsFormsApplication1.exe";

            //strCommandParameters are parameters to pass to program
            pProcess.StartInfo.Arguments = "";

            pProcess.StartInfo.UseShellExecute = false;

            //Set output of program to be written to process output stream
            pProcess.StartInfo.RedirectStandardOutput = true;

            //Optional
            pProcess.StartInfo.WorkingDirectory = Directory.GetCurrentDirectory() + "\\..\\Tray";

            //Start the process
            pProcess.Start();
        }

        private void button2_Click(object sender, EventArgs e)
        {            
            
            // The path to the key where Windows looks for startup applications
            RegistryKey rkApp = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", true);

            // Check to see the current state (running at startup or not)
            if (rkApp.GetValue("PyClientLauncher") != null)
            {
                // the application is registered
                rkApp.DeleteValue("PyClientLauncher", false);

            }
            button2.Enabled = false;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            button1.Enabled = true;

            // The path to the key where Windows looks for startup applications
            RegistryKey rkApp = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", true);

            // Check to see the current state (running at startup or not)
            if (rkApp.GetValue("PyClientLauncher") == null)
            {
                
            }
            else
            {
                // The value exists, the application is set to run at startup
                button2.Enabled = true; // can uninstall
            }

        }
    }
}
