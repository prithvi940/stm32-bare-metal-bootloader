import comms
import uart 
import time

firmware = "../app/firmware.bin"

RETX_MAX = 10

def main():
    try:
        uart.uart.reset_input_buffer()
        uart.uart.reset_output_buffer()
        TxPacket = bytes([0x09, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07])
        
        RxPacket = bytearray([])
        packet_type = 0x00
        with open(firmware, "rb") as file:
            content = file.read()
            RETX_COUNTER = 0
            for i in range(0, len(content), comms.PACKET_DATA_BYTES):
                TxPacket = comms.comms_create_packet(content[i:(i + comms.PACKET_DATA_BYTES)])
                while True:
                    comms.comms_send_packet(TxPacket)
                    RxPacket = comms.comms_read()
                    
                    if  RxPacket == None:
                        comms.comms_send_packet(comms.RETX_PACKET)
                        raise RuntimeError("Invalid packet received.")

                    packet_type = comms.packet_type(RxPacket)
                    if packet_type == comms.PacketType.ACK:
                        break

                    elif packet_type == comms.PacketType.RETX:
                        RETX_COUNTER = RETX_COUNTER + 1

                        if RETX_COUNTER >= RETX_MAX:
                            raise RuntimeError(f"Maximum retransmission limit ({RETX_MAX}) exceeded.")

                        continue

                    elif packet_type == comms.PacketType.NACK:
                        raise RuntimeError(f"Bootloader returned NACK. Aborting Update!!!")

        # while True:
        #     print(f"Sending: {TxPacket.hex(' ')}")
        #     comms.comms_send_packet(TxPacket)
        #     RxPacket = comms.comms_read()
        #     if RxPacket:
        #         print(f"Received: {RxPacket.hex(' ')}")
        #         ptype = comms.packet_type(RxPacket)
        #         print(f"Packet type: {ptype}")
        #     else:
        #         print("No response")

        print("Firmware update complete successfully.")

    finally:
        uart.uart_close()

if __name__ == "__main__":
    main()




    

