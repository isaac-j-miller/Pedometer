import serial, sys, glob, time


class SerialReader:
    def __init__(self):
        self.port = ''
        self.serialConnection = serial.Serial()
        print('reader init')

    def connect(self):
        if self.selectPort(self.listPorts()):
            print('port connected at', self.port)
        else:
            print('no appropriate ports')

    def isConnected(self):
        if self.serialConnection is None:
            return False
        else:
            return self.serialConnection.is_open

    def selectPort(self,port_list, trials=10):
        for port in port_list:
            currentSerial = serial.Serial(port, timeout=1)
            currentSerial.setDTR(1)
            time.sleep(2)
            for i in range(trials):
                data = self.handleInput(currentSerial, lambda f:None)
                if data is not None:
                    if 'ID' in data.keys():
                        if data['ID'] == 'ISAAC_PEDOMETER':
                            self.serialConnection=currentSerial
                            return True
                time.sleep(0.2)
            currentSerial.close()
        return False

    def handleInput(self, currentSerial, lostConnectionFunction):
        time.sleep(1)
        try:
            if currentSerial.in_waiting:
                buf = currentSerial.read(currentSerial.in_waiting)
                #print(buf)
                #currentSerial.flushInput()
                currentSerial.write('RD\r\n'.encode('utf-8'))
                try:
                    decoded = buf.decode('ANSI').replace('\r', '').split('\n')
                    for line in decoded:

                        if '!::' in line and '::!' in line:
                            beginning = line.index('!::')
                            end = line.index('::!')
                            line = line[beginning+3:end]
                            statements = line.split('::')
                            parsed = {}
                            for statement in statements:
                                if '=' in statement:
                                    split = statement.split('=')
                                    parsed[split[0]] = split[1]
                            return parsed

                except UnicodeDecodeError:
                    print('encode error. buf:', buf)
                    return None
            else:
                return None
        except serial.serialutil.SerialException:
            lostConnectionFunction()

    def listPorts(self):

        """ Lists serial port names

            :raises EnvironmentError:
                On unsupported or unknown platforms
            :returns:
                A list of the serial ports available on the system
        """
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')

        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        #print(result)
        return result

    def waitForConnection(self):
        while True:
            ports = self.listPorts()
            if self.selectPort(ports):
                print('port found')
                return
            print('port not found')

    def monitor(self):
        print('monitoring')
        self.serialConnection.timeout = 0
        while self.serialConnection.is_open:
            data = self.handleInput(self.serialConnection, self.waitForConnection)
            if data is not None:
                #print(data)
                if 'ID' in data.keys():
                    if data['ID'] == 'ISAAC_PEDOMETER':
                        try:

                            self.serialConnection.write('RD\r\n'.encode('utf-8'))
                            self.serialConnection.flushOutput()
                            #time.sleep(2)

                        except serial.serialutil.SerialException:
                            self.waitForConnection()


                if 'STEPS' in data.keys():
                    print("Total Steps:", data['STEPS'])

if __name__ == '__main__':
    reader = SerialReader()
    reader.waitForConnection()
    reader.monitor()
