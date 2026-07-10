import uart
import comms

firmware = "../app/firmware.elf"

lastSentPacket = bytearray([])
lastReceivedPacket = bytearray([])

def main():
    with open(firmware, "rb") as file:
        content = file.read()
        print(content)
        for i in range(len(content) / 8):
            lastSentPacket = comms.comms_create_packet(content[i:(i+8)])






    

