var client = null;


// called when the client connects
function onConnect() {
    // Once a connection has been made, make a subscription and send a message.
    console.log("onConnect");
    client.subscribe("World");
    message = new Paho.MQTT.Message("Hello");
    message.destinationName = "World";
    client.send(message);
}

// called when the client loses its connection
function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
        console.log("onConnectionLost:"+responseObject.errorMessage);
    }
}

// called when a message arrives
function onMessageArrived(message) {
    console.log("onMessageArrived:"+message.payloadString);
}

function mqttConnect(ip) {
    // set callback handlers
    client = new Paho.MQTT.Client(ip, Number(1883), "controller");
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    // connect the client
    client.connect({
        onSuccess:onConnect,
        mqttVersion: 4,
    });
}

$('.broker-ip').on('keyup', function (e) {
    if (e.key === 'Enter' || e.keyCode === 13) {
        var isValid = /^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)(\.(?!$)|$)){4}$/.test($(this).val());
        $('.test').text(isValid);
        if (isValid) {
            mqttConnect($(this).val());
        }
    }
});