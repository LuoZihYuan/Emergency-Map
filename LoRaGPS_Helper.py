"""
    This module helps deal with GPS signal read from serial port.
"""
# Python Standard Library
import datetime
from os import pardir, path
from threading import Thread
# Third Party Library
import serial.tools.list_ports
from serial.serialutil import SerialException

__history__ = path.abspath(path.join(__file__, pardir, 'History'))

class LoRaGPSListener(Thread):
    """ synchronously listens to GPS signal """
    def __init__(self, on_receive):
        super(LoRaGPSListener, self).__init__()
        self.on_receive = on_receive
        self._date = str(datetime.datetime.now().date())
        self.historyfile = open(path.join(__history__, self._date), "a", 1)

        self._terminate = False
        ports = serial.tools.list_ports.comports()
        for port in ports:
            if "LinkIt" in port.description:
                self.ser = serial.Serial(port.device, 9600)
                break
        else:
            self.historyfile.close()
            raise SerialException('not found: LinkIt serial')

    def run(self):
        """ Actual listening loop """
        self._terminate = False
        while self.ser:
            if self._terminate:
                break
            output = self.ser.readline().replace(b'\r\n', b'').decode("utf-8")
            recent_time = datetime.datetime.now()
            if self._date != str(recent_time.date()):
                self._date = str(recent_time.date())
                self.historyfile = open(path.join(__history__, self._date), "a", 1)
            self.historyfile.write(str(recent_time) + ' | Log: "' + output + '"\n')
            param = output.split(',')
            sid = param[0]
            gps = (float(param[1]), float(param[2]))
            self.on_receive(sid, gps)

    def stop(self):
        """ Stop listening to GPS """
        self._terminate = True
