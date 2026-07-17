import uart
from enum import Enum

class WritePacketState(Enum):
    LENGTH = 1
    DATA = 2
    CRC = 3
    SEND = 4
    DONE = 5

class PacketType(Enum):
    ACK = 1
    NACK = 2
    RETX = 3

def compute_crc(data):
    crc = 0
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x80:
                crc = ((crc << 1) ^ 0x07) & 0xFF
            else:
                crc = (crc << 1) & 0xFF
    return crc


PACKET_DATA_BYTES = 16
PACKET_LEN_BYTES = 1
PACKET_CRC_BYTES = 1
ACK_PACKET = bytes([0x01, 0x15]) + bytes([0xff] * 15)
RETX_PACKET = bytes([0x01, 0x19]) + bytes([0xff] * 15)
ACK_PACKET = ACK_PACKET + bytes([compute_crc(ACK_PACKET)])
RETX_PACKET = RETX_PACKET + bytes([compute_crc(RETX_PACKET)])

packet = bytearray([])
received_packet = bytearray([])
received_data = 0
state = 0

def comms_send_packet(outgoingPacket):
            uart.uart_write(outgoingPacket)


def comms_create_packet(BytesToSend):
    packet.clear()
    state = WritePacketState.LENGTH
    if len(BytesToSend) <= PACKET_DATA_BYTES:
        while state != WritePacketState.DONE:
            if state == WritePacketState.LENGTH:
                packetLength = len(BytesToSend)
                print(f"length of packet : {packetLength}")
                packet.append(packetLength)
                state = WritePacketState.DATA

            if state == WritePacketState.DATA:
                packet.extend(BytesToSend)
                if packetLength < PACKET_DATA_BYTES:
                    for i in range(PACKET_DATA_BYTES - packetLength):
                        packet.append(0xff)
                state = WritePacketState.CRC
                    
            if state == WritePacketState.CRC:
                crc = compute_crc(packet)
                packet.append(crc)
                state = WritePacketState.DONE
            
    return packet

def comms_read():
    received_packet = uart.uart_read()

    if len(received_packet) != 18:
        print("Short packet:", len(received_packet), received_packet.hex(" "))
        return None
        
    crc = received_packet[17]
    packet_data_length = received_packet[1]
    computed_crc = compute_crc(received_packet[0:17])
    print("Packet:", received_packet.hex(" "))
    print("CRC input:", received_packet[:17].hex(" "))
    print("CRC:", hex(compute_crc(received_packet[:17])))
    
    if computed_crc != crc:
        comms_send_packet(RETX_PACKET)
        return None
    
    ptype = packet_type(received_packet)
    if ptype != None:
        comms_send_packet(ACK_PACKET)

    return received_packet
    
def packet_type(rxpacket):
    data = rxpacket[1]
    if data == 0x15:
        return PacketType.ACK
    elif data == 0x19:
        return PacketType.RETX

