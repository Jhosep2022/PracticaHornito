<?php
// Configuración de la base de datos
$servername = "localhost";
$username = "root";
$password = "";
$port = 3306;
$dbname = "PRACTICA8db";

// Crear conexión
$conn = new mysqli($servername, $username, $password, $dbname, $port);

// Verificar conexión
if ($conn->connect_error) {
   die("Conexión fallida: " . $conn->connect_error);
}

// Verificar si la conexión es válida
if($conn){
    //Verificar si las variables POST están configuradas
    if (isset($_POST['NumReg']) && isset($_POST['Horno_Id']) && isset($_POST['Zona']) && isset($_POST['Fecha']) && isset($_POST['HoraActual']) && isset($_POST['Calefactor']) && isset($_POST['Enfriador'])&& isset($_POST['Set_Point']) && isset($_POST['TempProceso ']) && isset($_POST['SensorLM35'])) {

        // Obtener datos de POST
        $NumReg = $_POST['NumReg'];
        $Horno_Id = $_POST['Horno_Id'];
        $Zona = $_POST['Zona'];
        $Fecha = $_POST['Fecha'];
        $HoraActual = $_POST['HoraActual'];
        $Calefactor = $_POST['Calefactor'];
        $Enfriador = $_POST['Enfriador'];
        $Set_Point = $_POST['Set_Point'];
        $TempProceso = $_POST['TempProceso'];
        $SensorLM35 = $_POST['SensorLM35'];
    
        // Consulta SQL para insertar datos
        $sql = "INSERT INTO TablaHorno (NumReg, Horno_Id, Zona, Fecha, HoraActual, Calefactor, Enfriador, Set_Point,TempProceso, SensorLM35)
                VALUES ('$NumReg', '$Horno_Id', '$Zona', '$Fecha', '$HoraActual', '$Calefactor', '$Enfriador', '$Set_Point','$TempProceso','$SensorLM35')";
        
        //Ejecutar consulta y verificar resultado
        if($conn->query($sql) === TRUE){
            echo "Datos agregados correctamente";
        }else{
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    }else{
        echo "Error: Datos del POST no ingresados.";
    }
} else {
    echo "Error: Conexión con la base de datos no establecida.";
}

// Cerrar conexión
$conn->close();