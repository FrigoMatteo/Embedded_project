var client;
var id_user;

var last_freq = "";
var curr_freq = "";
var ackTimer;
var alarmFlashTimer;

var cur = "-- A";
var temp = "-- °C";
var lux = "-- Lux";
var hum = "--%";

const SENSOR_FREQ = 'sensor/freq';
const SENSOR_DATA = 'sensor/data';
const SENSOR_DATA_C = "sensor/data/curr";
const SENSOR_DATA_H = "sensor/data/hum";
const SENSOR_DATA_T = "sensor/data/temp";
const SENSOR_DATA_L = "sensor/data/lux";
const MIN_MAX = 'min_max';
const ACK = 'ack';
const ALARM = 'alarm';

const TO_MSP = 'msp';

document.addEventListener("DOMContentLoaded", function () {

    // Definisci i dettagli del server MQTT
    var server = "192.168.24.240";
    // var server = "172.20.10.5";

    var port = 9001;

    // Crea un client MQTT
    client = new Paho.MQTT.Client(server, port, "web-client");

    connectMQTT();

    client.onMessageArrived = function (message) {
        var receivedData = message.payloadString;
        var topic = message.destinationName;

        switch (topic) {
            case ALARM:
                if (receivedData == "!!") {
                    // Attiva il lampeggio quando arriva un messaggio di allarme
                    activateAlarmBanner();
                }
                if (receivedData == "AS") {
                    // Disattiva il lampeggio quando arriva un messaggio di stop allarme
                    deactivateAlarmBanner();
                }
                if (receivedData == "XX") {
                    publishMessage(TO_MSP, "MQ0");        // mess: Start Web
                }
                break;

            case SENSOR_FREQ:
                curr_freq = receivedData;
                last_freq = curr_freq;
                selectFrequencyButton(curr_freq);
                break;

            case SENSOR_DATA_C:
                cur = extractNumericValues(receivedData) + " A";
                updateTable();
                break;

            case SENSOR_DATA_H:
                hum = extractNumericValues(receivedData) + "%";
                updateTable();
                break;

            case SENSOR_DATA_T:
                temp = extractNumericValues(receivedData) + " °C";
                updateTable();
                break;

            case SENSOR_DATA_L:
                lux = extractNumericValues(receivedData) + " Lux";
                updateTable();
                break;

            case MIN_MAX:
                var possible_message = ['DD', 'MM', 'YY', 'AT'];

                // Verifica se la frequency è presente nell'array delle possibili frequenze
                if (possible_message.includes(receivedData)) {
                    fetchMinMaxData(receivedData);
                } else {
                    console.log("Error: Invalid request for min-max data.");
                }
                break;

            case ACK:
                clearTimeout(ackTimer);
                // Se riceve un messaggio di ack, deve corrispondere alla frequenza impostata dal bottone
                if (receivedData == ('AC' + curr_freq)) {
                    console.log("Frequency set correctly:", curr_freq);
                    last_freq = curr_freq;
                    showConfirmationMessage();
                } else {
                    console.log("Error: Frequency not set correctly");
                    curr_freq = last_freq;
                    showErrorMessage();
                }
                selectFrequencyButton(curr_freq);
                enableAllButtons();
                break;

            default:
                break;
        }
    };

    function updateTable() {
        document.getElementById("currentValue").textContent = cur;
        document.getElementById("temperatureValue").textContent = temp;
        document.getElementById("humidityValue").textContent = hum;
        document.getElementById("brightnessValue").textContent = lux;
    }
});

// Funzione di connessione al server MQTT
function connectMQTT() {
    if (!client.isConnected()) {
        console.log("Conncetion to the server MQTT...");
        client.connect({ onSuccess: onConnect });
    } else {
        console.log("The MQTT client is already connected.");
    }
}

// Callback di connessione al server MQTT
function onConnect() {
    console.log("Connected to the MQTT server.");
    subToTopic(SENSOR_FREQ);
    subToTopic(SENSOR_DATA_C);
    subToTopic(SENSOR_DATA_H);
    subToTopic(SENSOR_DATA_T);
    subToTopic(SENSOR_DATA_L);
    subToTopic(MIN_MAX);
    subToTopic(ACK);
    subToTopic(ALARM);
    publishMessage(TO_MSP, "SW");        // mess: Start Web
}

function disconnectMQTT() {
    if (client.isConnected()) {
        client.disconnect();
        console.log("Disconnected from the MQT server.");
    } else {
        console.log("The MQTT client is not connected.");
    }
}

// Subscribe/Unsubscribe al topic
function subToTopic(topic) {
    client.subscribe(topic);
    console.log('Subscribe to topic:', topic);
}

// Funzione per la pubblicazione di un messaggio MQTT
function publishMessage(topic, message) {
    var mqttMessage = new Paho.MQTT.Message(message);
    mqttMessage.destinationName = topic;
    client.send(mqttMessage);
    console.log("Sending message:", message + " on topic: " + topic);
}

function selectFrequency(frequency) {
    if (frequency != curr_freq) {

        disableAllButtons();

        curr_freq = frequency;
        publishMessage(TO_MSP, frequency);

        // Avvia il timer di 5 secondi per controllare l'ACK
        ackTimer = setTimeout(function () {
            // Mostra il messaggio di errore se l'ACK non è arrivato entro 5 secondi
            showErrorMessage();
            enableAllButtons();
            selectFrequencyButton(last_freq);
            curr_freq = last_freq;
        }, 5000);
    }
}

function selectFrequencyButton(frequency) {
    console.log("Selected frequency:", frequency);

    // Rimuovi la classe "selected" da tutti i pulsanti
    var buttons = document.querySelectorAll('.frequency-button');
    buttons.forEach(function (button) {
        button.classList.remove('selected');
    });

    // Dichiarazione dell'array delle possibili frequenze
    var possibleFreq = ['15', '30', '60', '1D'];

    // Verifica se la frequency è presente nell'array delle possibili frequenze
    if (possibleFreq.includes(frequency)) {
        // Aggiungi la classe "selected" al pulsante cliccato
        var clickedButton = document.getElementById(frequency);
        clickedButton.classList.add('selected');
    }
}

function disableAllButtons() {
    var buttons = document.querySelectorAll('.frequency-button');
    buttons.forEach(function (button) {
        button.disabled = true;
        button.classList.add('disabled');
    });
}

function enableAllButtons() {
    var buttons = document.querySelectorAll('.frequency-button');
    buttons.forEach(function (button) {
        button.disabled = false;
        button.classList.remove('disabled');
    });
}

function showConfirmationMessage() {
    var confirmationMessage = document.getElementById("confirmationMessage");
    confirmationMessage.style.display = "block";

    // Nascondi il messaggio dopo qualche secondo (ad esempio, 3 secondi)
    setTimeout(function () {
        confirmationMessage.style.display = "none";
    }, 3000);  // 3000 millisecondi = 3 secondi
}

function showErrorMessage() {
    var errorMessage = document.getElementById("errorMessage");
    errorMessage.style.display = "block";

    // Nascondi il messaggio dopo qualche secondo (ad esempio, 3 secondi)
    setTimeout(function () {
        errorMessage.style.display = "none";
    }, 3000);  // 3000 millisecondi = 3 secondi
}

// Funzione per attivare il banner e iniziare il lampeggiamento
function activateAlarmBanner() {
    var alarmBanner = document.getElementById("alarmBanner");

    // Resetta il timer se già attivo
    clearInterval(alarmFlashTimer);

    // Attiva la classe "active" per mostrare il banner
    alarmBanner.classList.add("active");

    // Imposta il timer per il lampeggiamento
    alarmFlashTimer = setInterval(function () {
        // Inverte i colori del banner
        alarmBanner.classList.toggle("inverted");
    }, 1000);  // Cambia ogni secondo (1000 millisecondi)
}

function deactivateAlarmBanner() {
    var alarmBanner = document.getElementById("alarmBanner");

    alarmBanner.classList.remove("active");
}

// Aggiungi questo blocco nel tuo codice
document.addEventListener("DOMContentLoaded", function () {
    var alarmBanner = document.getElementById("alarmBanner");

    // Aggiungi un gestore di eventi per l'evento di clic sul banner
    alarmBanner.addEventListener("click", function () {
        deactivateAlarmBanner();
    });
});

// Funzione per disattivare il banner
function deactivateAlarmBanner() {
    publishMessage(TO_MSP, "AS");        // mess: Alarm Stop
    var alarmBanner = document.getElementById("alarmBanner");

    // Disattiva la classe "active" per nascondere il banner
    alarmBanner.classList.remove("active");

    // Resetta il timer
    clearInterval(alarmFlashTimer);
}

function fetchMinMaxData(request) {
    console.log("Fetching min-max data for:", request);
    $.getJSON({
        type: "GET",
        url: "getMinMax",
        data: { type: request },
        success: function (response) {
            console.log("Servlet's response:", response);

            // Formatta il giorno, il mese e la corrente con due cifre
            var formattedMinHour = response.min_hh.padStart(2, '0');
            var formattedMaxHour = response.max_hh.padStart(2, '0');
            var formattedMinDay = response.min_dd.padStart(2, '0');
            var formattedMaxDay = response.max_dd.padStart(2, '0');
            var formattedMinMonth = response.min_mm.padStart(2, '0');
            var formattedMaxMonth = response.max_mm.padStart(2, '0');
            var formattedMinCurrent = response.min_curr.padStart(4, '0');
            var formattedMaxCurrent = response.max_curr.padStart(4, '0');

            var responseData = "mn" + formattedMinHour + formattedMinDay + formattedMinMonth + response.min_yy + formattedMinCurrent +
                "MX" + formattedMaxHour + formattedMaxDay + formattedMaxMonth + response.max_yy + formattedMaxCurrent;
            //publishMessage(TO_MSP, responseData);


            function publishWithDelay(message, delay) {
                setTimeout(function () {
                    publishMessage(TO_MSP, message);
                }, delay);
            }

            publishWithDelay("h" + "0" + formattedMinHour, 1000);
            publishWithDelay("d" + "0" + formattedMinDay, 2000);
            publishWithDelay("m" + "0" + formattedMinMonth, 3000);
            publishWithDelay("y" + "0" + response.min_yy, 4000);
            publishWithDelay("v" + "0" + formattedMinCurrent, 5000);
            publishWithDelay("h" + "1" + formattedMaxHour, 6000);
            publishWithDelay("d" + "1" + formattedMaxDay, 7000);
            publishWithDelay("m" + "1" + formattedMaxMonth, 8000);
            publishWithDelay("y" + "1" + response.max_yy, 9000);
            publishWithDelay("v" + "1" + formattedMaxCurrent, 10000);

            /*
            publishMessage(TO_MSP, "h" + "0" + formattedMinHour);
            publishMessage(TO_MSP, "d" + "0" + formattedMinDay);
            publishMessage(TO_MSP, "m" + "0" + formattedMinMonth);
            publishMessage(TO_MSP, "y" + "0" + response.min_yy);
            publishMessage(TO_MSP, "v" + "0" + formattedMinCurrent);
            publishMessage(TO_MSP, "h" + "1" + formattedMaxHour);
            publishMessage(TO_MSP, "d" + "1" + formattedMaxDay);
            publishMessage(TO_MSP, "m" + "1" + formattedMaxMonth);
            publishMessage(TO_MSP, "y" + "1" + response.max_yy);
            publishMessage(TO_MSP, "v" + "1" + formattedMaxCurrent);
*/

            console.log("Data to send to MSP:", responseData);

            // Aggiorna dinamicamente il contenuto della tabella
            $(".data-table td:nth-child(1)").eq(1).text("Min of " + request);
            $(".data-table td:nth-child(2)").eq(1).text("Max of " + request);
            $(".data-table td:nth-child(1)").eq(2).text(formattedMinHour + " hour");
            $(".data-table td:nth-child(2)").eq(2).text(formattedMaxHour + " hour");
            $(".data-table td:nth-child(1)").eq(3).text(formattedMinDay + "/" + formattedMinMonth + "/" + response.min_yy);
            $(".data-table td:nth-child(2)").eq(3).text(formattedMaxDay + "/" + formattedMaxMonth + "/" + response.max_yy);
            $(".data-table td:nth-child(1)").eq(4).text(formattedMinCurrent + " A");
            $(".data-table td:nth-child(2)").eq(4).text(formattedMaxCurrent + " A");
        },
        error: function (status, error) {
            console.error("Error during Ajax call:", status, error);
        }
    });
}

function extractNumericValues(inputString) {
    var numericValue = "";
    var numericValuesArray = [];

    for (var i = 0; i < inputString.length; i++) {
        var currentChar = inputString.charAt(i);

        // Verifica se il carattere è un numero o una virgola
        if (!isNaN(currentChar) || currentChar === ',') {
            numericValue += currentChar;
        } else {
            // Se il carattere non è un numero o una virgola, salva il valore e resetta la stringa numerica
            if (numericValue !== "") {
                numericValuesArray.push(parseFloat(numericValue));
                numericValue = "";
            }
        }
    }

    // Se c'è un valore numerico rimasto alla fine della stringa, salvalo
    if (numericValue !== "") {
        numericValuesArray.push(parseFloat(numericValue));
    }

    return numericValuesArray;
}

