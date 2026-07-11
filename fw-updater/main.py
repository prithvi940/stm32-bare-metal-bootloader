import comms
import uart 

firmware = "../app/firmware.elf"

RETX_MAX = 10

def main():
    try:
        TxPacket = bytearray([])
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
                    packet_type = comms.packet_type(RxPacket)
                    if  RxPacket == None:
                        raise RuntimeError("Invalid packet received.")
                    
                    if packet_type == comms.PacketType.ACK:
                        break
                    elif packet_type == comms.PacketType.RETX:
                        RETX_COUNTER = RETX_COUNTER + 1

                        if RETX_COUNTER >= RETX_MAX:
                            raise RuntimeError(f"Maximum retransmission limit ({RETX_MAX}) exceeded.")

                        continue
                    elif packet_type == comms.PacketType.NACK:
                        raise RuntimeError(f"Bootloader returned NACK. Aborting Update!!!")
        print("Firmware update complete successfully.")
    
    finally:
        uart.uart_close()





    

