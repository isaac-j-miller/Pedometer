import serial, sys, glob, time
import tkinter as tk
import asyncio
 

class Application(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.parent=parent
        self.window = tk.Toplevel(self)
        self.startStopButton = tk.Button(self.window, text="Start", command=self.startCollection)
        self.steps = tk.StringVar()
        self.status = tk.StringVar()
        self.status.set('no connection')
        self.statusLabel = tk.Label(self.window, text="", textvariable=self.status)
        self.valueBox = tk.Entry(self.window,text ="",textvariable= self.steps, state=tk.DISABLED, disabledforeground="BLACK")
        self.window.title('Step Counter')
        self.window.grid()
        self.statusLabel.grid(row=0, column=0, columnspan=2, sticky="NSEW")
        self.startStopButton.grid(row=1, column=0, sticky="NSEW")
        self.valueBox.grid(row=1, column=1, sticky="NSEW")
        self.reader = SerialReader(self)
        self.parent.withdraw()
        self.window.protocol("WM_DELETE_WINDOW", self.smoothExit) 

    def smoothExit(self):
        self.killAll()
        quit()

    def killAll(self):
        try:
            for task in asyncio.all_tasks(asyncio.get_running_loop()):
                task.cancel()
        except RuntimeError:
            pass

    def startCollection(self):
        self.updateStatus('searching for pedometer...')
        self.startStopButton.configure(state=tk.DISABLED)
        self.reader.waitForConnection()
        self.updateButton()
        self.updateStatus('pedometer found')
        asyncio.run(self.reader.monitor())

    def stopCollection(self):
        self.updateStatus('no connection')
        self.updateButton()
        self.updateText('')
        self.killAll()

    def updateStatus(self, newStatus):
        self.status.set(newStatus)
        self.statusLabel.update()
        self.updateAll()

    def updateText(self, newText):
        self.steps.set(newText)
        self.valueBox.update()
        self.updateAll()

    def updateButton(self):
        self.startStopButton.configure(state=tk.NORMAL)
        if self.startStopButton.cget('text') == "Start":
            self.startStopButton.configure(text='Stop', command=self.stopCollection)
        else:
            self.startStopButton.configure(text='Start', command=self.startCollection)
        self.updateAll()
        self.startStopButton.update()
    
    def updateAll(self):
        self.window.update_idletasks()


class SerialReader:

    def __init__(self, parent):
        self.parent = parent
        self.port = ''
        self.serialConnection = serial.Serial()
        self.tkRoot = self.parent.parent
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
            print(port)
            currentSerial = serial.Serial(port, timeout=1)
            self.tkRoot.update()
            time.sleep(2)
            for i in range(trials):
                data = self.handleInput(currentSerial, lambda f:None)
                self.tkRoot.update()
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
                currentSerial.write('RD\r\n'.encode('utf-8'))
                try:
                    decoded = buf.decode('utf-8').replace('\r', '').split('\n')
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
            This method was partially stolen from
            https://stackoverflow.com/questions/12090503/listing-available-com-ports-with-python
            :raises EnvironmentError:
                On unsupported or unknown platforms
            :returns:
                A list of the serial ports available on the system
        """
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            #ports = glob.glob('/dev/tty[A-Za-z]*')
            ports = ['/dev/serial/by-id/usb-FTDI_FT231X_USB_UART_DN06ACF8-if00-port0']
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')
        result = []
        for port in ports:
            self.tkRoot.update()
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException) as a:
                pass
        return result

    def waitForConnection(self):
        while True:
            self.tkRoot.update()
            ports = self.listPorts()
            if self.selectPort(ports):
                print('port found')
                return
            print('port not found')
    
    async def monitor(self):
        print('monitoring')
        self.serialConnection.timeout = 0
        in_waiting = await self.getInWaiting()

        while in_waiting:
            print('in waiting',in_waiting)
            data = self.handleInput(self.serialConnection, self.waitForConnection)
            if data is not None:
                if 'ID' in data.keys():
                    if data['ID'] == 'ISAAC_PEDOMETER':
                        try:

                            self.serialConnection.write('RD\r\n'.encode('utf-8'))
                            self.serialConnection.flushOutput()

                        except serial.serialutil.SerialException:
                            self.waitForConnection()

                if 'STEPS' in data.keys():
                    string = 'Total Steps : {}'.format(data['STEPS'])
                    print(string)
                    self.parent.updateText(string)

            in_waiting = await self.getInWaiting()

        in_waiting = await self.getInWaiting()

     
    async def getInWaiting(self):
        while not self.serialConnection.in_waiting:
            await asyncio.sleep(0.1)
            self.tkRoot.update()
        return self.serialConnection.in_waiting
     
if __name__ == '__main__':
    #reader = SerialReader()
    #reader.waitForConnection()
    #reader.monitor()
    root = tk.Tk()
    app = Application(root)
    app.mainloop()

