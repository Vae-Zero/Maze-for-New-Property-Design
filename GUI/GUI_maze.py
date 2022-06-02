import sys
import numpy as np
from tkinter import *
from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor, tksupport

window = Tk()
window.title("Maze")
window.geometry("640x360")
tksupport.install(window)


def on_closing():
    reactor.stop()
    window.destroy()

window.protocol("WM_DELETE_WINDOW", on_closing)


class EchoClientDatagramProtocol(DatagramProtocol):

    def startProtocol(self):
        self.transport.connect('192.168.43.220', 9527)
        # self.transport.write(CurCmd)

    def sendDatagram(self, msg):
        self.transport.write(msg)

    def datagramReceived(self, datagram, address):
        print(datagram.decode('utf-8'))

PPP = EchoClientDatagramProtocol()
t = reactor.listenUDP(0, PPP)

def MLB_Clicked(event):

    CmdMsg = ""

    if np.abs(event.x - 155) < 64:
        if np.abs(event.y - 170) < 32:
            CmdMsg = "E"
        elif np.abs(event.y - 235) < 32:
            CmdMsg = "N"
        elif np.abs(event.y - 300) < 32:
            CmdMsg = "H"
    elif (event.x - 450) ** 2 + (event.y - 100) ** 2 < 2500 :
        CmdMsg = "U"
    elif (event.x - 530) ** 2 + (event.y - 180) ** 2 < 2500 :
        CmdMsg = "R"
    elif (event.x - 450) ** 2 + (event.y - 260) ** 2 < 2500 :
        CmdMsg = "D"
    elif (event.x - 370) ** 2 + (event.y - 180) ** 2 < 2500 :
        CmdMsg = "L"

    if CmdMsg != "" :
        print(CmdMsg)
        PPP.sendDatagram(CmdMsg.encode('utf-8'))

canvas = Canvas(window, width = 640, height = 360)
canvas.pack()

img0 = PhotoImage(file = "back_x.png")
canvas.create_image(0, 0, anchor = NW, image = img0)

img1 = PhotoImage(file = "RBT_Easy.png")
img2 = PhotoImage(file = "RBT_Easy(1).png")
canvas.create_image(155, 170, anchor = CENTER, image = img1, activeimage = img2)

img3 = PhotoImage(file = "RBT_Norm.png")
img4 = PhotoImage(file = "RBT_Norm(1).png")
canvas.create_image(155, 235, anchor = CENTER, image = img3, activeimage = img4)

img5 = PhotoImage(file = "RBT_Hard.png")
img6 = PhotoImage(file = "RBT_Hard(1).png")
canvas.create_image(155, 300, anchor = CENTER, image = img5, activeimage = img6)

img7 = PhotoImage(file = "RBT_Up.png")
img8 = PhotoImage(file = "RBT_Up(1).png")
canvas.create_image(450, 100, anchor = CENTER, image = img7, activeimage = img8)

img9 = PhotoImage(file = "RBT_Right.png")
imga = PhotoImage(file = "RBT_Right(1).png")
canvas.create_image(530, 180, anchor = CENTER, image = img9, activeimage = imga)

imgb = PhotoImage(file = "RBT_Down.png")
imgc = PhotoImage(file = "RBT_Down(1).png")
canvas.create_image(450, 260, anchor = CENTER, image = imgb, activeimage = imgc)

imgd = PhotoImage(file = "RBT_Left.png")
imge = PhotoImage(file = "RBT_Left(1).png")
canvas.create_image(370, 180, anchor = CENTER, image = imgd, activeimage = imge)

canvas.bind("<Button-1>", MLB_Clicked)

#mainloop()
reactor.run()
