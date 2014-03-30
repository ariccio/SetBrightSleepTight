using System;
using System.Collections.Generic;
using System.Drawing;
//using System.Text.RegularExpressions;
using System.Windows.Forms;
//Consider using AppDomains for parsing?
//EL GPL LICENCE, MOFOs
[assembly: CLSCompliant( true )]
namespace DisplayBrightnessConsole {

    public static class BaseConst {
        static public int GetHourBright( int position ) {
            return HourBright[ position ];
            }
        static public double GetVersion( ) {
            return Version;
            }
        private static readonly int[]  HourBright = { 22, 44, 54, 76, 83, 92, 97, 100 };
        private const double Version    = 0.4;
        }

    internal class Program {

        private static int GetBrightness( ) {
            System.Management.ManagementScope               s   = null;
            System.Management.SelectQuery                   q   = null;
            System.Management.ManagementObjectSearcher      mos = null;
            System.Management.ManagementObjectCollection    moc = null;
            int curBrightness = 0;
                
            try {
                s   =     new System.Management.ManagementScope( "root\\WMI" );
                q   =     new System.Management.SelectQuery( "WmiMonitorBrightness" );
                mos =     new System.Management.ManagementObjectSearcher( s, q );
                moc =     mos.Get( );

                foreach ( System.Management.ManagementObject o in moc ) {
                    curBrightness = Convert.ToInt32( o.GetPropertyValue( "CurrentBrightness" ) );
                    break; //only work on the first object
                    }
                }
            finally {
                moc.Dispose( );
                mos.Dispose( );
                }
                return curBrightness;
            }

        private static int[ ] GetBrightnessLevels( ) {

            System.Management.ManagementScope               s   = null;
            System.Management.SelectQuery                   q   = null;
            System.Management.ManagementObjectSearcher      mos = null;
            System.Management.ManagementObjectCollection    moc = null;

            int[] BrightnessLevels = new int[ 0 ];//store result
            try {
                s   = new System.Management.ManagementScope( "root\\WMI" );
                q   = new System.Management.SelectQuery( "WmiMonitorBrightness" );
                mos = new System.Management.ManagementObjectSearcher( s, q );
                moc = mos.Get( );

                foreach ( System.Management.ManagementObject o in moc ) {
                    Console.Write( " " + o + "; " );
                    BrightnessLevels = ( int[ ] )o.GetPropertyValue( "Level" );
                    break; //only work on the first object
                    }                
                }
            finally {
                mos.Dispose( );
                moc.Dispose( );
                }
            return BrightnessLevels;
            }

        private static void displayWMIInfo( ) {
            System.Management.ManagementScope               s   = null;
            System.Management.SelectQuery                   q   = null;
            System.Management.ManagementObjectSearcher      mos = null;
            System.Management.ManagementObjectCollection    moc = null;
            
            int[] BrightnessLevels = null;

            try {
                s   = new System.Management.ManagementScope( "root\\WMI" );
                q   = new System.Management.SelectQuery( "WmiMonitorBrightness" );
                mos = new System.Management.ManagementObjectSearcher( s, q );
                moc = mos.Get( );

                BrightnessLevels = new int[ 0 ];//store result

                foreach ( System.Management.ManagementObject o in moc ) {
                    Console.WriteLine( "PATH: " + o.Path );
                    List<string> newget = new List<string>( );
                    newget.Add( o.GetText( System.Management.TextFormat.Mof ) );
                    Console.WriteLine( "--------------------------------" );
                    foreach ( string n in newget ) {
                        Console.WriteLine( n );
                        }
                    }
                }

            finally {
                moc.Dispose( );
                mos.Dispose( );
                }
            }

        private static void setBrightnessLevelFromGivenArgumentOnCommandLine( int targetBrightness ) {
            if ( targetBrightness > 100 || targetBrightness < 0 ) //handles the wtf case where brightness is more than 100 or less than zero
                                {
                Console.WriteLine( "What the hell?? Target brightness \"" + targetBrightness + "\" out of bounds!" );

                int i = ( targetBrightness > 100 ? 100 : 0 );
                SetBrightness( i );

                Console.WriteLine( "Brightness set to: \"" + i + "\" instead!" );
                return;
                }
            else {
                SetBrightness( targetBrightness );
                //Console.WriteLine( "Current Brightness: " + GetBrightness( ) );
                }
            }

        private static void setBrightnessLevelToDefaultForCurrentTime( NotifyIcon tray ) {
            
            Console.WriteLine( "Current brightness: " + GetBrightness( ) );

            int hour = DateTime.Now.Hour;
            int min  = DateTime.Now.Minute;

            mapBright( hour, min );
            tray.ShowBalloonTip( 1000, "SetBright? SleepTight!", "version " + BaseConst.GetVersion( ) + " works, yo", ToolTipIcon.Info );
            }

        private static void writeSupportedBrightnessLevelsToConsole( ) {
            int[] BrightnessLevels = GetBrightnessLevels( );
            Console.Write( "This monitor supports " + ( BrightnessLevels.Length ) + " brightness levels: " );

            foreach ( int b in BrightnessLevels ) {
                Console.Write( b.ToString( ) + ", " );
                }
            }

        private static void preformTestSequence( ) {
            int[] BrightnessLevels = GetBrightnessLevels( );
            int   initbright       = GetBrightness( );

            foreach ( int b in BrightnessLevels ) {
                SetBrightness( b );
                Console.WriteLine( "Brightness->" + b.ToString( ) );
                System.Threading.Thread.Sleep( 100 );
                }

            for ( int i = 0; i < 17; i++ ) {
                for ( int m = 0; m < 60; m += 30 ) {
                    Console.WriteLine( " mapBright(" + i + ", " + m + ");" );
                    mapBright( i, m );
                    }
                }
            SetBrightness( initbright );
            }

        private static void writeCurrentTimeToConsole( ) {
            int hour = DateTime.Now.Hour;
            int min  = DateTime.Now.Minute;
            Console.WriteLine( "Current time is: " + hour + ":" + min );
            }
        [STAThread]
        private static void Main( string[ ] args ) {
            Console.WriteLine( "version " + BaseConst.GetVersion() + "\n" );
            NotifyIcon tray = null;
            try {
                tray         = new NotifyIcon( );
                tray.Icon    = SystemIcons.Application;
                tray.Visible = true;

                if ( args.Length == 0 ) {
                    setBrightnessLevelToDefaultForCurrentTime( tray );
                    }

                else if ( args[ 0 ] == "-getlevels" ) {
                    writeSupportedBrightnessLevelsToConsole( );
                    }

                else if ( args[ 0 ] == "-getwmiinfo" ) {
                    displayWMIInfo( );
                    }

                else if ( args[ 0 ] == "-loop" ){
                    preformTestSequence( );
                    }

                else if ( args[ 0 ] == "-time" ) {
                writeCurrentTimeToConsole( );
                    }

                else//assumes number was input as an input, i.e. "brightness.exe 100"
                {
                    int targetBrightness;
                    if ( int.TryParse( args[ 0 ], out targetBrightness ) ) {
                        setBrightnessLevelFromGivenArgumentOnCommandLine( targetBrightness );
                        }
                    else {
                        Console.WriteLine( "Invalid input!, please try again" );
                        return;
                        } 
                    }
                }
            finally {
                tray.Dispose( );
                }
            }

        private static void mapBright( int hourNow, int minuteNow ) {
            if ( hourNow < 8 || hourNow > 19 ) {
                Console.Write( "night time! Setting brightness -> 0! " );
                SetBrightness( 0 );
                }
            switch ( hourNow ) {
                case 8:
                    SetBrightness( BaseConst.GetHourBright(     ( ( minuteNow < 30 ) ? 0 : 1 ) ) );//if not yet 8:30, 22; else 44
                    break;

                case 9:
                    SetBrightness( BaseConst.GetHourBright( 1 + ( ( minuteNow < 30 ) ? 0 : 1 ) ) );
                    break;

                case 10:
                    SetBrightness( BaseConst.GetHourBright( 2 + ( ( minuteNow < 30 ) ? 0 : 1 ) ) );
                    break;

                case 11:
                    SetBrightness( BaseConst.GetHourBright( 3 + ( ( minuteNow < 30 ) ? 0 : 1 ) ) );
                    break;

                case 12:
                    SetBrightness( BaseConst.GetHourBright( 7 + ( ( minuteNow < 30 ) ? 0 : -1 ) ) );
                    break;

                case 13:
                    SetBrightness( BaseConst.GetHourBright( 7 + ( ( minuteNow < 30 ) ? 0 : -1 ) ) );
                    break;

                case 14:
                    SetBrightness( BaseConst.GetHourBright( 6 + ( ( minuteNow < 30 ) ? 0 : -1 ) ) );
                    break;

                case 15:
                    SetBrightness( BaseConst.GetHourBright( 5 + ( ( minuteNow < 30 ) ? 0 : -1 ) ) );
                    break;
                }
            }

        private static void SetBrightness( int targetBrightness ) {
            NotifyIcon tray = null;
            System.Management.ManagementScope               s   = null;
            System.Management.SelectQuery                   q   = null;
            System.Management.ManagementObjectSearcher      mos = null;
            System.Management.ManagementObjectCollection    moc = null;

            try {
                Console.WriteLine( "Setting Brightness: " + targetBrightness );

                tray         = new NotifyIcon( );
                tray.Icon    = SystemIcons.Application;
                tray.Visible = true;

                String targetBrightnessStr = targetBrightness.ToString( );

                //tray.ShowBalloonTip( 100, "Setting Brightness: ", targetBrightnessStr, ToolTipIcon.Info );

                s   =     new System.Management.ManagementScope( "root\\WMI" );
                q   =     new System.Management.SelectQuery( "WmiMonitorBrightnessMethods" );
                mos =     new System.Management.ManagementObjectSearcher( s, q );
                moc =     mos.Get( );

                foreach ( System.Management.ManagementObject o in moc )//output current brightness
                    {
                    o.InvokeMethod( "WmiSetBrightness", new Object[ ] { UInt32.MaxValue, targetBrightness } ); //note the reversed order - won't work otherwise!
                    break; //only work on the first object
                    }

                }
            finally {
                moc.Dispose( );
                mos.Dispose( );
                tray.Dispose( );
                }
            }
        }
    }