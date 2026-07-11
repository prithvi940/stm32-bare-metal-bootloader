import serial

BAUDRATE = 115200
PORT = "/dev/ttyACM0"

uart = serial.Serial(port=PORT, 
                     baudrate=BAUDRATE, 
                     bytesize=serial.EIGHTBITS, 
                     parity=serial.PARITY_NONE, 
                     stopbits=1, 
                     timeout=1)

uart2 = serial.Serial()


def uart_read(length=18):
    return uart.read(length) 
    

def uart_write(outgoingBuffer):
        uart.write(outgoingBuffer)
 
def uart_close():
      uart.close()