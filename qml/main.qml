/***************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import diy 1.0
Window {
    id: wroot
    visible: true
    width: 720
    height: 720
    title: qsTr("ToyClawMachine")

    MainProcess {
        id: application
        onCoinChanged: {
            lblCoin.text = coin;
        }
        onMachineStateChanged: {
            lblMachineState.text = state;
        }
        onActuatorStepChanged: {
            canvas.rX = ratioX;
            canvas.rY = ratioY;
            canvas.rZ = ratioZ;
            canvas.rClaw = ratioClaw;
            canvas.requestPaint();
        }
        onLedButtonChanged: {
            ledBtn.color = active?"green":"white";
        }
        onLedJoystickChanged: {
            ledJS.color = active?"orange":"white";
        }
    }

    Button{
        x: 350
        y: 593
        text: "Right"
        font.pointSize: 12
        onPressed: {
            application.handleAxisChanged(1,0);
        }
        onReleased: {
            application.handleAxisChanged(0,0);
        }
    }
    Button{
        x: 146
        y: 593
        text: "Left"
        font.pointSize: 12
        onPressed: {
            application.handleAxisChanged(-1,0);
        }
        onReleased: {
            application.handleAxisChanged(0,0);
        }
    }
    Button{
        x: 249
        y: 663
        text: "Down"
        font.pointSize: 12
        onPressed: {
            application.handleAxisChanged(0,-1);
        }
        onReleased: {
            application.handleAxisChanged(0,0);
        }
    }
    Button{
        x: 249
        y: 525
        text: "Up"
        font.pointSize: 12
        onPressed: {
            application.handleAxisChanged(0,1);
        }
        onReleased: {
            application.handleAxisChanged(0,0);
        }
    }

    Button {
        x: 469
        y: 669
        width: 105
        height: 40
        text: "Claw"
        font.pointSize: 12
        onPressed: {
            application.handleButtonSignal(true);
        }
        onReleased: {
            application.handleButtonSignal(false);
        }
    }

    Button {
        id: button
        x: 601
        y: 669
        text: qsTr("Coin")
        font.pointSize: 12
        onPressed: {
            application.handleCoinSignal(true);
        }
        onReleased: {
            application.handleCoinSignal(false);
        }
    }

    Label {
        id: lblCoin
        x: 601
        y: 617
        width: 100
        height: 46
        text: qsTr("0")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 12
    }

    Rectangle {
        id: rectangle
        x: 10
        y: 12
        width: 700
        height: 500
        color: "#84af40"
        Canvas {
            id: canvas
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 49
            anchors.topMargin: 71
            width: 605
            height: 390
            property color strokeStyle:  Qt.darker(fillStyle, 1.4)
            property color fillStyle: "#b40000" // red
            property int lineWidth: 3
            property bool fill: true
            property bool stroke: true
            property real alpha: 1.0
            property real scale : 1
            property real rotate : 60
            antialiasing: true

            property real rX: 0
            property real rY: 0
            property real rZ: 0
            property real rClaw: 0
            onPaint: {
                var ctx = canvas.getContext('2d');
                ctx.reset();
                ctx.globalAlpha = canvas.alpha;
                ctx.strokeStyle = canvas.strokeStyle;
                ctx.fillStyle = canvas.fillStyle;
                ctx.lineWidth = canvas.lineWidth;

                //O----A-------H------D Axis X
                //|   /       /      /
                //|  /       /      /
                //| M-------I------N
                //|/       /|     /
                //B-------K-|----C---
                //Axis Y    |
                //          |
                //         <P>
                var OB = 100
                var OA = OB*Math.tan(rotate);
                var OD = 600;
                var BC = OD - OA;
                var Mx = OA*(1-rY)
                var My = OB*rY
                var OH = OA + (OD-OA)*rX
                var BK = (OD-OA)*rX
                var Nx = Mx + (OD-OA)
                var Ny = My
                var Ix = Mx + (OD-OA)*rX
                var Iy = My
                var Px = Ix
                var Py = Iy + OB*2*rZ

                // Draw Top
                ctx.moveTo(OA,0)
                ctx.lineTo(OD,0)
                ctx.lineTo(BC,OB)
                ctx.lineTo(0,OB)
                ctx.lineTo(OA,0)
                ctx.stroke();

                ctx.fillStyle = "green";
                // Draw X
                ctx.moveTo(Mx, My);
                ctx.lineTo(Nx, Ny);
                ctx.fill();
                //                // Draw Y
                ctx.moveTo(OH,0);
                ctx.lineTo(BK,OB);
                ctx.stroke();
                // Draw CrossLine
                ctx.fillStyle  = "red";
                ctx.rect(Ix-10,Iy-10,20,20);
                ctx.stroke();
                // Draw Z
                ctx.fillStyle  = "green";
                ctx.moveTo(Ix,Iy);
                ctx.lineTo(Px,Py);
                ctx.stroke();
                // Draw Craw
                ctx.fillStyle  = "black";
                ctx.beginPath();
                ctx.moveTo(Px,Py);
                ctx.lineTo(Px+50,Py);
                ctx.arc(Px, Py,
                        50, 0, Math.PI/10+ Math.PI/3 * rClaw);
                ctx.lineTo(Px,Py);
                ctx.arc(Px, Py,
                        50, Math.PI-( Math.PI/10+ Math.PI/3 * rClaw), Math.PI);
                ctx.lineTo(Px,Py);
                ctx.stroke();
            }
        }

        Label {
            id: lblMachineState
            x: 8
            y: 8
            width: 682
            height: 57
            color: "#ffffff"
            text: qsTr("MACHINE_STATE")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 20


        }
    }

    Label {
        id: label1
        x: 469
        y: 513
        width: 239
        height: 79
        text: qsTr("00:00")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 20
    }

    CheckBox {
        id: checkBox
        x: 10
        y: 533
        width: 121
        height: 165
        text: qsTr("Power")
        font.pointSize: 14
        onCheckedChanged: {
            if(checked) application.startService();
            else application.stopService();
        }
    }

    Rectangle {
        id: ledJS
        x: 268
        y: 585
        width: 60
        height: 60
        color: "#ffffff"
        radius: 30
        border.color: "#000000"
    }

    Rectangle {
        id: ledBtn
        x: 492
        y: 598
        width: 60
        height: 60
        color: "#ffffff"
        radius: 30
        border.color: "#000000"
    }
    Component.onCompleted: {
        canvas.requestPaint();
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:0.75}
}
##^##*/
