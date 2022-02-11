const mqtt = require('mqtt')
const client = mqtt.connect('127.24.0.4:1883', {
  keepalive: 60,
  clientId: 'clientId',
  protocolId: 'MQTT',
  protocolVersion: 4,
  clean: true,
  reconnectPeriod: 1000,
  connectTimeout: 30 * 1000,
})  // create a client

$('.broker-ip').on('keyup', function (e) {
    if (e.key === 'Enter' || e.keyCode === 13) {
        var ip = $(this).val().trim();
        var isValid = /^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)(\.(?!$)|$)){4}$/.test(ip);
        $('.test').text(isValid);
        if (isValid) {
            mqttConnect(ip);
        }
    }
});