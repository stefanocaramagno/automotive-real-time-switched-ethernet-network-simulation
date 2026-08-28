package rtn_01;

// Custom channel type based on DatarateChannel.
//
// NOTE: To enable Bit Error Rate (BER), uncomment the line 'ber = default(1e-8)'.

channel CustomChannel extends ned.DatarateChannel
{
    parameters:
        datarate = default(10Gbps);
        // ber = default(1e-8);
}