# M7 - Smart Techo

Proyecto de la materia Sistemas Operativos Avanzados.

# Presentación

  ## Integrantes:   

    Miranda Sergio Javier       35634266    semiranda@alumno.unlam.edu.ar
    Rodriguez Ezequiel Nicolás  40135570    ezeqrodriguez@alumno.unlam.edu.ar
    Meza Julian                 39463982    jmeza@alumno.unlam.edu.ar
    Mariano Emanuel Toloza      37113832    mariatoloza@alumno.unlam.edu.ar


# Descripcion del proyecto:

  "SMART TECHO” es un sistema diseñado para controlar un techo corredizo de manera automatizada. Su activación se realiza mediante un sensor de luz y lluvia, así como a través de un pulsador manual ubicado cerca del techo corredizo o mediante una aplicación móvil conectada al sistema mediante una conexión inalámbrica mediante un módulo de Bluetooth. 
  
  Estos medios de control permiten al techo corredizo ajustar su posición, ya sea abriéndose o cerrándose, en respuesta a las condiciones climáticas o a la elección del usuario. El pulsador manual y la aplicación móvil ofrecen al cliente la capacidad de tomar decisiones sobre el estado deseado del techo corredizo.


# Descripcion de partes:

  	Arduino UNO R3
	Fuente de alimentación Digital Regulable(5V)
	4 resistencias de 120 Ω y una 10 ㏀
	1 módulo de conexión a Bluetooth.
	1 pulsadores (señal digital)
	2 fin de carrera (señal digital)
	1 led blanco (señal digital)
	1 led rojo (señal digital)
	Motor de corriente continua (señal analógica)  
	Protoboard “Placa de pruebas”
	Sensor de lluvia (señal analogica)
	Sensor de luz ambiental (señal analogica)
	Controlador de motor de puente H (L293D) (señal digital)
    Módulo Bluetooth HC06

 
  ## Diagrama de Conexión
  ![](/diagramas/conexiones_01.png)
  
  ## Dispositivo físico
  ![](/diagramas/fisico_01.jpg)
  ![](/diagramas/fisico_02.jpg)
  ### Sensores y actuadores
  ![](/diagramas/fisico_03.jpg)
  

## Diagrama de estados
En el diagrama se puede observar el estado del dispositivo en un momento dado, y los posibles estados al cual puede pasar. Por estado se debe entender como las diferentes combinaciones de información que la maquina puede mantener.

![](/diagramas/máquina_de_estado_01.png)




# Descripción de la Aplicación Android:
Esta parte del proyecto consiste en una aplicación móvil que nos sirve de ayuda y de complemento para ejecutar, consultar y poner en practica todas funcionalidades. 
Inicialmente, al ejecutar la aplicación, se presentará una interfaz que muestra el logotipo y el nombre de la aplicación. En la pantalla principal, se proporcionarán todas las funcionalidades y recursos necesarios para operar el techo automatizado mediante la aplicación móvil. Los usuarios tendrán la capacidad de controlar el techo corredizo de forma manual, así como visualizar el estado de operación y acceder a una sección informativa que muestra los valores porcentuales de la intensidad luminosa registrada por el sensor de luz.

|||
|---|---|
| ![](/diagramas/app_01.jpg) | ![](/diagramas/app_02.jpg) |
|![](/diagramas/app_03.jpg)| |
|![](/diagramas/app_04.jpg)|![](/diagramas/app_05.jpg)|





# Manual de uso:
Al iniciar la aplicación SmartTecho, nos mostrara una pantalla en la cual tenemos que seleccionar nuestro dispositivo bluetooth (Se deberá contar con un dispositivo compatible con bluetooth 2.0 o superior)
Una vez que se ha establecido la conexión, es posible visualizar el estado actual del sistema, así como el valor porcentual proporcionado por el sensor de luz. Desde la aplicación, se ofrece la capacidad de abrir y cerrar manualmente el techo corredizo a través de botones diseñados específicamente para este propósito. Además, se brinda la opción de restablecer el sistema para que vuelva a operar en modo automático.

El sistema implementado en este trabajo de IoT opera mediante la utilización de dos modos de funcionamiento distintos: manual y automatico

## Modo automatico
En este modo de operación, el sistema se basa en sensores de lluvia y luz para determinar la apertura o cierre automático del techo. Cuando el sensor de lluvia detecta un aumento en los valores de tolerancia predefinidos, el techo se cerrará. Del mismo modo, cuando los valores de luz indican una iluminación baja, lo cual denota la noche, el techo también se cerrará, y se activará una luz en el estado cerrado.


## Modo manual
En el modo manual, la apertura y cierre del techo son completamente controlados por el usuario, quien decide si mantenerlo cerrado o abierto, independientemente de los valores de los sensores de lluvia o luz. En este estado, se encenderá un LED rojo, indicando que el sistema se encuentra en modo manual.
Es posible realizar la transición del modo manual al modo automático cuando el usuario lo desee, ya sea mediante el pulsador manual o a través de la aplicación móvil.

Puede acceder al simulador de Tinkercad en el siguiente enlace: 
[SMART_TECHO | Tinkercad ](https://www.tinkercad.com/things/joPthXBBa8I)


## Desafios

Durante el desarrollo de nuestro proyecto embebido, enfrentamos desafíos significativos que afectaron su funcionamiento. La transición entre los modos automático y manual resultó problemática al intentar implementar estados compartidos. Para superar este problema, abordamos las partes automática y manual como módulos separados pero interconectados, como se muestra en el diagrama de estado disponible en este repositorio.

Además, la alimentación eléctrica del circuito presentó un desafío importante. La fuente de 5V inicial no era suficiente para alimentar todos los componentes. Con la ayuda del profesor Esteban Carnuccio, identificamos que el consumo de energía del controlador del puente H L293D era la causa. En colaboración, implementamos una solución utilizando una fuente de alimentación adicional de 5V exclusivamente para el controlador, mientras que el resto del circuito se alimentaba con otra fuente de 5V separada.

Estas soluciones permitieron superar los desafíos encontrados y mejorar el rendimiento y la funcionalidad general del proyecto embebido.

# Conclusiones

En conclusión, durante el desarrollo de nuestro proyecto embebido, hemos adquirido una valiosa experiencia que nos será útil para nuestro proyecto final de carrera. Ahora comprendemos cómo desarrollar y codificar sistemas embebidos utilizando herramientas como Tinkercad. También hemos adquirido conocimientos en el desarrollo de aplicaciones para Android utilizando Java y Android Studio. Estas habilidades y conocimientos adquiridos serán de gran utilidad en nuestras futuras carreras profesionales.


## Referencias

1.  E. A. Carnuccio, W. Valiente, and M. Volker, “Creación de entorno integrado para Sistemas Embebidos,” ReDDI: Revista Digital del Departamento de Ingeniería, vol. 6, no. 1, pp. 1–8, 2021, doi: https://doi.org/10.54789/reddi.6.1.2.
2. Tutoriales Arduino V5.pdf, “Tutoriales Arduino V5.pdf,” Google Docs, 2019. https://drive.google.com/file/d/1d8EEDzAmauhnJJ7IHh8odY-qLOX13oRu/view (accessed May 02, 2023).
3. L. Llamas, “Detector de lluvia con Arduino y sensor FC-37 o YL-83,” Luis Llamas, Feb. 13, 2016. https://www.luisllamas.es/arduino-lluvia/ (accessed May 02, 2023).
4. pamela, “¿Qué es el Puente H L283D? El Driver puente H para motores L293D es un circuito que tiene en su Leer más,” Blog Arduino, LabVIEW y Electrónica, Jan. 22, 2021. https://electronicamade.com/puente-h-l293d/ (accessed May 02, 2023)
5. “Ejemplos Android/Arduino-AndroidBluethoot/Android-Code · master · so-unlam / Material-SOA · GitLab,” GitLab, 2023. https://gitlab.com/so-unlam/Material-SOA/-/tree/master/Ejemplos%20Android/Arduino-AndroidBluethoot/Android-Code  (accessed Jul. 04, 2023).
6. Jason, “Android Tutorial: Implement A Shake Listener – Jason McReynolds,” Jasonmcreynolds.com, 2015. http://jasonmcreynolds.com/?p=388 (accessed Jul. 04, 2023).
