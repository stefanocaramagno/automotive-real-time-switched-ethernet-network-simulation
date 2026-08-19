package rtn_01;

// Vehicle Sensor and Control Network.
// This scenario models an Ethernet-based network interconnecting multiple vehicle devices
// via Ethernet switches.
//
// NOTE: Ports are numbered starting from 0 up to (portCount - 1).

import ned.IdealChannel;
import rtn_01.EtherSwitch;
import rtn_01.CustomChannel;

network Scenario2
{
    @display("bgb=900,500");

    submodules:
        // Switches
        sw1: EtherSwitch {
            parameters:
                portCount = 8;
                @display("p=148,391");
        }
        sw2: EtherSwitch {
            parameters:
                portCount = 6;
                @display("p=248,116");
        }
        sw3: EtherSwitch {
            parameters:
                portCount = 6;
                @display("p=745,116");
        }
        sw4: EtherSwitch {
            parameters:
                portCount = 6;
                @display("p=645,391");
        }

		// Lidar sensors
        ld1: EthNode { @display("p=88,324"); }
        ld2: EthNode { @display("p=168,149"); }

		// Control unit
        cu: EthNode { @display("p=322,188"); }

		// Ultrasonic sensors
        us1: EthNode { @display("p=49,392"); }
        us2: EthNode { @display("p=187,67"); }
        us3: EthNode { @display("p=835,94"); }
        us4: EthNode { @display("p=723,440"); }

		// Multimedia Equipment
        me: EthNode { @display("p=561,440"); }

		// Speakers
        s1: EthNode { @display("p=298,351"); }
        s2: EthNode { @display("p=385,149"); }
        s3: EthNode { @display("p=684,188"); }
        s4: EthNode { @display("p=578,294"); }

		// Head unit
        hu: EthNode { @display("p=242,306"); }

		// Camera Module
        cm1: EthNode { @display("p=88,455"); }

        // Rear sensors
        rs1: EthNode { @display("p=617,149"); }
        rs2: EthNode { @display("p=507,351"); }

		// Rear camera
        rc: EthNode { @display("p=816,174"); }

		// Telematics module
        tlm: EthNode { @display("p=223,440"); }

    // Keyword "allowunconnected" disables the connectivity check for all compound module gates
    connections allowunconnected:

        // Switch1 <-> Switch2 (port 0)
        sw1.channelIn[0] <-- CustomChannel <-- sw2.channelOut[0];
        sw1.channelOut[0] --> CustomChannel --> sw2.channelIn[0];

        // Switch2 <-> Switch3 (port 1)
        sw2.channelIn[1] <-- CustomChannel <-- sw3.channelOut[1];
        sw2.channelOut[1] --> CustomChannel --> sw3.channelIn[1];

		// Switch3 <-> Switch4 (port 0)
        sw3.channelIn[0] <-- CustomChannel <-- sw4.channelOut[0];
        sw3.channelOut[0] --> CustomChannel --> sw4.channelIn[0];

        // Switch4 <-> Switch1 (port 1)
        sw4.channelIn[1] <-- CustomChannel <-- sw1.channelOut[1];
        sw4.channelOut[1] --> CustomChannel --> sw1.channelIn[1];

		// Lidar1 <-> Switch1 (port 2)
        ld1.channelIn <-- CustomChannel <-- sw1.channelOut[2];
        ld1.channelOut --> CustomChannel --> sw1.channelIn[2];

        // Lidar2 <-> Switch2 (port 2)
        ld2.channelIn <-- CustomChannel <-- sw2.channelOut[2];
        ld2.channelOut --> CustomChannel --> sw2.channelIn[2];

        // Control Unit <-> Switch2 (port 3)
        cu.channelIn <-- CustomChannel <-- sw2.channelOut[3];
        cu.channelOut --> CustomChannel --> sw2.channelIn[3];

 		// Ultrasonic Sensor 1 <-> Switch1 (port 3)
        us1.channelIn <-- CustomChannel <-- sw1.channelOut[3];
        us1.channelOut --> CustomChannel --> sw1.channelIn[3];

 		// Ultrasonic Sensor 2 <-> Switch2 (port 4)
        us2.channelIn <-- CustomChannel <-- sw2.channelOut[4];
        us2.channelOut --> CustomChannel --> sw2.channelIn[4];

	 	// Ultrasonic Sensor 3 <-> Switch3 (port 2)
        us3.channelIn <-- CustomChannel <-- sw3.channelOut[2];
        us3.channelOut --> CustomChannel --> sw3.channelIn[2];

		// Ultrasonic Sensor 4 <-> Switch4 (port 2)
        us4.channelIn <-- CustomChannel <-- sw4.channelOut[2];
        us4.channelOut --> CustomChannel --> sw4.channelIn[2];

		// Multimedia Equipment <-> Switch4 (port 3)
        me.channelIn <-- CustomChannel <-- sw4.channelOut[3];
        me.channelOut --> CustomChannel --> sw4.channelIn[3];

        // Speaker1 <-> Switch1 (port 4)
        s1.channelIn <-- CustomChannel <-- sw1.channelOut[4];
        s1.channelOut --> CustomChannel --> sw1.channelIn[4];

		// Speaker2 <-> Switch2 (port 5)
        s2.channelIn <-- CustomChannel <-- sw2.channelOut[5];
        s2.channelOut --> CustomChannel --> sw2.channelIn[5];

		// Speaker3 <-> Switch3 (port 3)
        s3.channelIn <-- CustomChannel <-- sw3.channelOut[3];
        s3.channelOut --> CustomChannel --> sw3.channelIn[3];

        // Speaker4 <-> Switch4 (port 4)
        s4.channelIn <-- CustomChannel <-- sw4.channelOut[4];
        s4.channelOut --> CustomChannel --> sw4.channelIn[4];

        // Head Unit <-> Switch1 (port 5)
        hu.channelIn <-- CustomChannel <-- sw1.channelOut[5];
        hu.channelOut --> CustomChannel --> sw1.channelIn[5];

        // Camera Module <-> Switch1 (port 6)
        cm1.channelIn <-- CustomChannel <-- sw1.channelOut[6];
        cm1.channelOut --> CustomChannel --> sw1.channelIn[6];

        // Rear Sensor 1 <-> Switch3 (port 4)
        rs1.channelIn <-- CustomChannel <-- sw3.channelOut[4];
        rs1.channelOut --> CustomChannel --> sw3.channelIn[4];

        // Rear Sensor 2 <-> Switch4 (port 5)
        rs2.channelIn <-- CustomChannel <-- sw4.channelOut[5];
        rs2.channelOut --> CustomChannel --> sw4.channelIn[5];

        // Rear Camera <-> Switch3 (port 5)
        rc.channelIn <-- CustomChannel <-- sw3.channelOut[5];
        rc.channelOut --> CustomChannel --> sw3.channelIn[5];

		// Telematics Module <-> Switch1 (port 7)
        tlm.channelIn <-- CustomChannel <-- sw1.channelOut[7];
        tlm.channelOut --> CustomChannel --> sw1.channelIn[7];
}