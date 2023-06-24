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
  ![](/diagramas/fisico_01.png)
  ![](/diagramas/fisico_02.png)
  

## Diagrama de estados
En el diagrama se puede observar el estado del dispositivo en un momento dado, y los posibles estados al cual puede pasar. Por estado se debe entender como las diferentes combinaciones de información que la maquina puede mantener.

![](/diagramas/máquina_de_estado_01.png)




# Descripción de la Aplicación Android:
Esta segunda parte del proyecto consiste en una aplicación móvil que nos sirve de ayuda y de complemento para ejecutar, consultar y poner en practica todas funcionalidades. 
En principio, al iniciar la aplicación veremos una pantalla que contiene tanto el logo como el nombre de nuestra aplicación. ....
![](/diagramas/app_01.jpg)


![](/diagramas/app_02.jpg)






# Manual de uso:
Al iniciar la aplicación SmartTecho, nos mostrara una pantalla en la cual tenemos que seleccionar nuestro dispositivo bluetooth.... 
(Se deberá contar con un dispositivo compatible con bluetooth 2.0 o superior)


## Modo automatico
En este modo de operación, el sistema se basa en sensores de lluvia y luz para determinar la apertura o cierre automático del techo. Cuando el sensor de lluvia detecta un aumento en los valores de tolerancia predefinidos, el techo se cerrará. Del mismo modo, cuando los valores de luz indican una iluminación baja, lo cual denota la noche, el techo también se cerrará, y se activará una luz en el estado cerrado.


## Modo manualmente
En el modo manual, la apertura y cierre del techo son completamente controlados por el usuario, quien decide si mantenerlo cerrado o abierto, independientemente de los valores de los sensores de lluvia o luz. En este estado, se encenderá un LED rojo, indicando que el sistema se encuentra en modo manual.
Es posible realizar la transición del modo manual al modo automático cuando el usuario lo desee, ya sea mediante el pulsador manual o a través de la aplicación móvil.




# Conclusiones




