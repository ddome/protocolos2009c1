####################################################################
		Gu’a de configuracion de MovieStore
####################################################################

Para instalaci—n leer INSTALL.txt

Indice

1.Programa cliente
2.Programa servidor
3.Programa servidor lookup
4.Programa servidor payment

####################################################################
			  1. Programa cliente
####################################################################

El archivo client.config contiene los ips y puertos necesarios para 
establecer una comunicaci—n con el servidor. Para mas informaci—n
editar el archivo leyendo las instrucciones all’ presentes.

####################################################################
			  2. Programa servidor
####################################################################

El archivo server.config contiene los ips y puertos necesarios para 
establecer una comunicaci—n con el cliente y el servidor lookup. 
Para mas informaci—n editar el archivo leyendo las instrucciones 
all’ presentes.

El archivo movies_location.txt contiene la informaci—n necesaria
para ubicar las pel’culas dentro del sistema y almacenar su 
informaci—n con el formato 
"path;titulo;genero;descripcion;minutos;valor".

El archivo tickets_free contiene informaci—n sobre los nœmeros de
tickets ya generados. Este archivo se autogenera y no
es modificable.

El archivo tickets_data contiene informaci—n sobre la asociaci—n
de pel’culas y tickets generados. Este archivo se autogenera y no
es modificable.


####################################################################
		       3. Programa servidor Lookup
####################################################################

El archivo lookup.config contiene los ips y puertos necesarios para 
establecer una comunicaci—n con el servidor. 
Para mas informaci—n editar el archivo leyendo las instrucciones 
all’ presentes.

El archivo servers_location.txt contiene la informaci—n de los
servidores de pago disponibles con el formato
"nombre;ip;host;clave de encriptacion;TTL"

####################################################################
		       4. Programa servidor payment
####################################################################

El archivo payment.config contiene los ips y puertos necesarios para 
establecer una comunicaci—n con el servidor. 
Para mas informaci—n editar el archivo leyendo las instrucciones 
all’ presentes.

El archivo payment.cvs contiene la informaci—n de las cuentas
manejadas por el servidor de pago. Para mas informaci—n editar
el archivo leyendo las instrucciones all’ presentes.




