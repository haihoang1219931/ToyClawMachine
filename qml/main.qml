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
    
    ApplicationController {
        id: application
        onCoinChanged: {
            lblCoin.text = coin;
        }
        onMachineStateChanged: {
            lblMachineState.text = state;
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
        y: 608
        width: 105
        height: 101
        text: "Claw"
        font.pointSize: 12
        onClicked: {
            application.handleClawPressed();
        }
    }
    
    Button {
        id: button
        x: 601
        y: 669
        text: qsTr("Coin")
        font.pointSize: 12
        onClicked: {
            application.handleCoinPushed();
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
        width: 698
        height: 495
        color: "#84af40"
        
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
}

/*##^##
Designer {
    D{i:0;formeditorZoom:0.75}
}
##^##*/
