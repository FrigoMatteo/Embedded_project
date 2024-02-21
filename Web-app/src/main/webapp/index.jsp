<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<html>
<head>
    <title>IoT Dashboard</title>
    <link rel="stylesheet" type="text/css" href="css/styles.css">
    <script src="js/mqttws31.js"></script>
    <script src="js/mqtt-client.js"></script>
    <script src="https://code.jquery.com/jquery-3.6.4.min.js"></script>
</head>
<body>

<div id="confirmationMessage" style="display: none;">Frequency set correctly!</div>
<div id="errorMessage" style="display: none;">Error: Frequency not set correctly.<br>Please try again.</div>
<div id="alarmBanner" class="alarm-banner">
    <p>Error: Current issue</p>
</div>

<div class="header">
    <div class="container">
        <div class="frequency-column">
        <h2>Change sensor frequency</h2>
        <button id="15" class="frequency-button" onclick="selectFrequency('15')">15 min</button>
        <button id="30" class="frequency-button" onclick="selectFrequency('30')">30 min</button>
        <button id="60" class="frequency-button" onclick="selectFrequency('60')">1 hour</button>
        <button id="1D" class="frequency-button" onclick="selectFrequency('1D')">1 day</button>
    </div>


    <div class="table-container">
        <table class="data-table">
            <tr>
                <th>Register Current</th>
                <th>Theoretical Current</th>
            </tr>
            <tr>
                <td>null</td>
                <td>null</td>
            </tr>
        </table>
        <table class="data-table">
            <tr>
                <td>Min of </td>
                <td>Max of </td>
            </tr>
            <tr>
                <td>-- hour</td>
                <td>-- hour</td>
            </tr>
            <tr>
                <td>--/--/--</td>
                <td>--/--/--</td>
            </tr>
            <tr>
                <td>-- A</td>
                <td>-- A</td>
            </tr>
        </table>
    </div>

    <div class="values-table">
        <h2>Last Values Taken</h2>
        <table class="data-table">
            <tr>
                <td id="currentLabel">Current</td>
                <td id="currentValue">-- A</td>
            </tr>
            <tr>
                <td id="temperatureLabel">Temperature</td>
                <td id="temperatureValue">-- °C</td>
            </tr>
            <tr>
                <td id="humidityLabel">Humidity</td>
                <td id="humidityValue">--%</td>
            </tr>
            <tr>
                <td id="brightnessLabel">Brightness</td>
                <td id="brightnessValue">-- Lux</td>
            </tr>
        </table>
    </div>
</div>

</body>
</html>
