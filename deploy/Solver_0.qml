import QtQuick 2.0
import QtQuick.Controls 2.0

Item {
    id: item
    anchors.fill: parent
    //x: 300
    //y: 300
    signal qmlSignal(string msg)

    Grid {
           id: grid
           //x: 200
           //y: 100
           anchors.fill: parent
           transformOrigin: Item.Center
           spacing: 4
           rows: params.dimArgs + params.dimParams + 2 + 2 * Math.max(params.dimParams, params.dimArgs)
           columns: 2

           Text
           {
               id: eps
               text: "Epsilon"
               width: 100
           }

           Rectangle
           {
               border.width: 2
               border.color: "black"
               width: 200
               height: eps.height
               clip: true
               TextInput
               {
                   objectName: "eps"
                   width: parent.width - 4
                   anchors.horizontalCenter: parent.horizontalCenter
                   validator: DoubleValidator{}
               }
           }

           Text
           {
               id: pararg
               text: "Params or args"
               width: 100
           }

           Rectangle
           {
               border.width: 2
               border.color: "black"
               width: 200
               height: pararg.height
               clip: true
               TextInput
               {
                   objectName: "pararg"
                   width: parent.width - 4
                   anchors.horizontalCenter: parent.horizontalCenter
                   validator: DoubleValidator{}
               }
           }

           Text
           {
               id: arg1
               text: "Arg1"
               width: 100
           }

           Rectangle
           {
               border.width: 2
               border.color: "black"
               width: 200
               height: arg1.height
               clip: true
               TextInput
               {
                   objectName: "arg1"
                   width: parent.width - 4
                   anchors.horizontalCenter: parent.horizontalCenter
                   validator: DoubleValidator{}
               }
           }

           Text
           {
               id: arg2
               text: "Arg2"
               width: 100
           }

           Rectangle
           {
               border.width: 2
               border.color: "black"
               width: 200
               height: arg2.height
               clip: true
               TextInput
               {
                   objectName: "arg2"
                   width: parent.width - 4
                   anchors.horizontalCenter: parent.horizontalCenter
                   validator: DoubleValidator{}
               }
           }

           Text
           {
               id: param1
               text: "Param1"
               width: 100
           }

           Rectangle
           {
               border.width: 2
               border.color: "black"
               width: 200
               height: param1.height
               clip: true
               TextInput
               {
                   objectName: "param1"
                   width: parent.width - 4
                   anchors.horizontalCenter: parent.horizontalCenter
                   validator: DoubleValidator{}
               }
           }

           Text
           {
               id: param2
               text: "Param2"
               width: 100
           }

           Rectangle
           {
               border.width: 2
               border.color: "black"
               width: 200
               height: param2.height
               clip: true
               TextInput
               {
                   objectName: "param2"
                   width: parent.width - 4
                   anchors.horizontalCenter: parent.horizontalCenter
                   validator: DoubleValidator{}
               }
           }

           Text
           {
               id: lb1
               text: "Left border1"
               width: 100
           }

           Rectangle
           {
               border.width: 2
               border.color: "black"
               width: 200
               height: lb1.height
               clip: true
               TextInput
               {
                   objectName: "lb1"
                   width: parent.width - 4
                   anchors.horizontalCenter: parent.horizontalCenter
                   validator: DoubleValidator{}
               }
           }

           Text
           {
               id: lb2
               text: "Left border2"
               width: 100
           }

           Rectangle
           {
               border.width: 2
               border.color: "black"
               width: 200
               height: lb2.height
               clip: true
               TextInput
               {
                   objectName: "lb2"
                   width: parent.width - 4
                   anchors.horizontalCenter: parent.horizontalCenter
                   validator: DoubleValidator{}
               }
           }

           Text
           {
               id: rb1
               text: "Right border1"
               width: 100
           }

           Rectangle
           {
               border.width: 2
               border.color: "black"
               width: 200
               height: rb1.height
               clip: true
               TextInput
               {
                   objectName: "rb1"
                   width: parent.width - 4
                   anchors.horizontalCenter: parent.horizontalCenter
                   validator: DoubleValidator{}
               }
           }

           Text
           {
               id: rb2
               text: "Right border2"
               width: 100
           }

           Rectangle
           {
               border.width: 2
               border.color: "black"
               width: 200
               height: rb2.height
               clip: true
               TextInput
               {
                   objectName: "rb2"
                   width: parent.width - 4
                   anchors.horizontalCenter: parent.horizontalCenter
                   validator: DoubleValidator{}
               }
           }
    }

    Button {
        x: 495
        y: 406
        text: "OK"
	onClicked: item.qmlSignal("SOLVE")
    }

}
