using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text.RegularExpressions;
using System.Windows.Forms;

//EL GPL LICENCE, MOFOs
[assembly: CLSCompliant( true )]
namespace DisplayBrightnessConsole {

    public static class BaseConst {
        //BaseConst( ) { }
        public const double Version = 0.3;
        static public int GetHourBright( int position ) {
            return HourBright[ position ];
            }
        private static readonly int[] HourBright = { 22, 44, 54, 76, 83, 92, 97, 100 };
        }



    internal class Program {

        private static int GetBrightness( ) {
        System.Management.ManagementScope               s   = null;
        System.Management.SelectQuery                   q   = null;
        System.Management.ManagementObjectSearcher      mos = null;
        System.Management.ManagementObjectCollection    moc = null;
        int curBrightness = 0;
            try {
                //System.Management.ManagementScope               s   =     new System.Management.ManagementScope( "root\\WMI" );
                //System.Management.SelectQuery                   q   =     new System.Management.SelectQuery( "WmiMonitorBrightness" );
                
                s   =     new System.Management.ManagementScope( "root\\WMI" );
            
                q   =     new System.Management.SelectQuery( "WmiMonitorBrightness" );

                mos =     new System.Management.ManagementObjectSearcher( s, q );

                moc =     mos.Get( );

                foreach ( System.Management.ManagementObject o in moc ) {

                    curBrightness = Convert.ToInt32( Convert.ToString( o.GetPropertyValue( "CurrentBrightness" ) ) );

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
            int[] retBrightnessLevels = null;
            try {
                s = new System.Management.ManagementScope( "root\\WMI" );

                q = new System.Management.SelectQuery( "WmiMonitorBrightness" );

                mos = new System.Management.ManagementObjectSearcher( s, q );

                moc = mos.Get( );



                byte[] BrightnessLevels = new byte[ 0 ];//store result

                foreach ( System.Management.ManagementObject o in moc ) {

                    Console.Write( " " + o + "; " );

                    BrightnessLevels = ( byte[ ] )o.GetPropertyValue( "Level" );

                    break; //only work on the first object

                    }

                retBrightnessLevels = new int[ BrightnessLevels.Length ];

                for ( int i  = 0; i < BrightnessLevels.Length; i++ ) {

                    retBrightnessLevels[ i ] = Convert.ToInt32( BrightnessLevels[ i ] );

                    }
                }
            finally {
                mos.Dispose( );
                moc.Dispose( );
                }
            return retBrightnessLevels;
            }

        
        private static void displayWMIInfo( ) {

            System.Management.ManagementScope               s   = null;
            System.Management.SelectQuery                   q   = null;
            System.Management.ManagementObjectSearcher      mos = null;
            System.Management.ManagementObjectCollection    moc = null;
            int[] BrightnessLevels = null;
            try {
                s = new System.Management.ManagementScope( "root\\WMI" );

                q = new System.Management.SelectQuery( "WmiMonitorBrightness" );

                mos = new System.Management.ManagementObjectSearcher( s, q );

                moc = mos.Get( );


                BrightnessLevels = new int[ 0 ];//store result

                foreach ( System.Management.ManagementObject o in moc ) {
                    Console.WriteLine( o.ClassPath );
                    Console.WriteLine( o.Container );
                    Console.WriteLine( o.Options );
                    Console.WriteLine( o.Path );
                    Console.WriteLine( o.Properties );
                    Console.WriteLine( o.Qualifiers );
                    Console.WriteLine( o.Scope );
                    Console.WriteLine( o.Site );
                    Console.WriteLine( o.SystemProperties );

                    List<string> newget = new List<string>( );
                    newget.Add( o.GetText( System.Management.TextFormat.Mof ) );
                    newget.Add( o.GetText( System.Management.TextFormat.CimDtd20 ) );
                    newget.Add( o.GetText( System.Management.TextFormat.WmiDtd20 ) );

                    string newget3 = o.GetText( System.Management.TextFormat.WmiDtd20 );
                    string[] newgetwords3 = Regex.Split( newget3, ">" );
                    string[][] lesstrings = new string[ 1 ][ ] { newgetwords3 };

                    Console.WriteLine( );
                    Console.WriteLine( );

                    for ( int b = 0; b < lesstrings.Length; b++ ) {
                        foreach ( string n in lesstrings[ b ] ) {
                            Console.WriteLine( ">" );
                            Console.Write( n );
                            }
                        }
                    Console.WriteLine( o.GetPropertyQualifierValue( "Active", "CIMTYPE" ) );
                    Console.WriteLine( o.GetPropertyQualifierValue( "InstanceName", "CIMTYPE" ) );
                    Console.WriteLine( o.GetPropertyValue( "__RELPATH" ) );
                    Console.WriteLine( o.GetPropertyValue( "__PATH" ) );
                    }
                }
            finally {
                moc.Dispose( );
                mos.Dispose( );
                }
            }

        [STAThread]
        private static void Main( string[ ] args ) {
            Console.WriteLine( "version " + BaseConst.Version + "\n" );
            NotifyIcon tray = null;
            try {
                tray = new NotifyIcon( );
                tray.Icon = SystemIcons.Application;
                tray.Visible = true;

                #region noargs

                if ( args.Length == 0 ) {
                    tray.ShowBalloonTip( 1000, "SetBright? SleepTight!", "version " + BaseConst.Version + " works, yo", ToolTipIcon.Info );
                    Console.WriteLine( "DEBUG: Current brightness: " + GetBrightness( ) );
                    int hour = DateTime.Now.Hour;
                    int min = DateTime.Now.Minute;
                    mapBright( hour, min );
                    System.Threading.Thread.Sleep( 1000 );
                    }

                #endregion noargs

                #region getlevels

                else if ( args[ 0 ] == "-getlevels" ) {
                    int[] BrightnessLevels = GetBrightnessLevels( );
                    Console.Write( "This monitor supports " + ( BrightnessLevels.Length ) + " brightness levels: " );
                    foreach ( int b in BrightnessLevels ) {
                        Console.Write( b.ToString( ) + ", " );
                        }
                    }

                #endregion getlevels

                else if ( args[ 0 ] == "-getwmiinfo" ) {
                    displayWMIInfo( );
                    }

                #region loop

                else if ( args[ 0 ] == "-loop" )//test sequence
                {
                    int[] BrightnessLevels = GetBrightnessLevels( );
                    int initbright = GetBrightness( );

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

                #endregion loop

                #region time

                else if ( args[ 0 ] == "-time" ) {
                    int hour = DateTime.Now.Hour;
                    int min = DateTime.Now.Minute;
                    Console.WriteLine( "Current time is: " + hour + ":" + min );
                    }

                #endregion time

                else//assumes number was input as an input, i.e. "brightness.exe 100"
                {
                    int targetBrightness;
                    bool res = int.TryParse( args[ 0 ], out targetBrightness );

                    //parse switch value
                    if ( res == false ) {
                        Console.WriteLine( "Invalid input!, please try again" );
                        } if ( res == true ) {
                        if ( targetBrightness > 100 || targetBrightness < 0 ) //handles the wtf case where brightness is more than 100 or less than zero
                        {
                            Console.WriteLine( "What the hell?? Target brightness \"" + targetBrightness + "\" out of bounds!" );
                            int i = ( targetBrightness > 100 ? 100 : 0 );

                            //byte l = ( ( byte )i );//required explicit type conversion
                            SetBrightness( i );
                            Console.WriteLine( "Brightness set to: \"" + i + "\" instead!" );
                            return;
                            }

                        //byte h = ( ( byte )targetBrightness );//required explicit type conversion
                        SetBrightness( targetBrightness );
                        Console.WriteLine( "Current Brightness: " + GetBrightness( ) );
                        }
                    }
                }
            finally {
                tray.Dispose( );
                }
            }

        private static void mapBright( int hourNow, int minuteNow ) {
            if ( hourNow < 8 || hourNow > 15 ) {
                Console.Write( "night time! Setting brightness -> 0! " );
                SetBrightness( 0 );
                }
            switch ( hourNow ) {
                case 8:
                    SetBrightness( BaseConst.GetHourBright( ( ( minuteNow < 30 ) ? 0 : 1 ) ) );//if not yet 8:30, 22; else 44
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

            tray = new NotifyIcon( );

            tray.Icon = SystemIcons.Application;

            tray.Visible = true;

            String targetBrightnessStr = targetBrightness.ToString( );

            tray.ShowBalloonTip( 1000, "Setting Brightness: ", targetBrightnessStr, ToolTipIcon.Info );

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