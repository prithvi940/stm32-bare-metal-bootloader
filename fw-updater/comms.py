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

def compute_crc(packetList, length=17):
    crc = 0
    for i in range(length):
        crc = crc ^ packetList[i]
        for j in range(8):
            if crc & 0x80:
                crc = (crc << 1) ^ 0x07 & 0xff
            else:
                crc <<= 1
    return crc & 0xff


PACKET_DATA_BYTES = 16
PACKET_LEN_BYTES = 1
PACKET_CRC_BYTES = 1

packet = bytearray([])
received_packet = bytearray([])
received_data = 0
state = 0

def comms_send_packet(outgoingPacket):
            uart.uart_write(outgoingPacket)


def comms_create_packet(BytesToSend):
    packet.clear()
    state = WritePacketState.LENGTH
    if BytesToSend < PACKET_DATA_BYTES:
        while state != WritePacketState.DONE:
            if state == WritePacketState.LENGTH:
                packetLength = len(BytesToSend)
                packet.append(packetLength)
                state = WritePacketState.DATA

            if state == WritePacketState.DATA:
                packet.extend(BytesToSend)
                if packetLength < PACKET_DATA_BYTES:
                    for i in range(PACKET_DATA_BYTES - packetLength):
                        packet.append(0xff)
                state = WritePacketState.CRC
                    
            if state == WritePacketState.CRC:
                crc = compute_crc(packet, packetLength + 1)
                packet.append(crc)
                state = WritePacketState.DONE
            
    return packet

def comms_read():
    received_packet.clear()
    received_packet = uart.uart_read()
    crc = received_packet[17]
    computed_crc = compute_crc(received_packet[0:17])
    if computed_crc != crc:
        return None
    return received_packet
    
def packet_type(rxpacket):
    data = rxpacket[1]
    if  rxpacket[1] == 0x59:
        return PacketType.NACK
    elif rxpacket[1] == 0x15:
        return PacketType.ACK
    elif rxpacket[1] == 0x19:
        return PacketType.RETX

