# MEW TUTORIAL
## 1.Mi primera ventana
*Inicialización*

```cpp

	auto maybe_ws = MEW::WindowSystem::make();
	if (!maybe_ws)
	{
		return -1;
	}
	auto ws = maybe_ws.value();
	std::string title = "Window Example Triangle";
	auto maybe_w = MEW::Window::make(1280, 720, title, ws);
	if (!maybe_w) {
		return -1;
	}
	MEW::Window w = maybe_w.value();


```

*Bucle*

```cpp
while(){
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();

		//Código
			
		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();

}

```
*Destruir*
```cpp



w.~Window();
ws.~WindowSystem();
```

## X ECSManager
* Gestor de entidades, como crearlas,y acceder a ellas *

*Primero la creación del ecs*
```cpp

	MEW::ECSManager ecs;


	//Añadimos los componentes existentes o que vayamos a utilizar (pueden ser     //creados por el usuario)
	ecs.add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::RenderComponent>();
	ecs.add_component_type<MEW::LightComponent>();
	ecs.add_component_type<MEW::CameraComponent>();
	ecs.add_component_type<MEW::IdentityComponent>();


```
* Crear una entidad

```cpp 

	size_t miku = ecs.create_entity("miku");
	ecs.add_component<MEW::RenderComponent>(miku);
	ecs.add_component<MEW::TransformComponent>(miku);

```

* Hay clases como Light que en su constructor hacen uso del ecs para crear una entidad, así que como consejo si una clase necesita el ecs consulta si tiene una variable de tipo size_t que se llame entidad. Y entonces se creará con lo necesario por defecto.

* Acceder a un componente y cambiarlo

```cpp

MEW::TransformCompoennt *transformHolder = ecs.get_component<MEW::TransformComponent>(miku);

transformHolder->scale_x = 10.0f;

```
* Aviso el ecs se autogestiona los punteros, entonces no deberemos almacenar los componentes que nos devuelve. Siempre deberemos llamar al get_component.

## X ImGui + Inspector

* Para implementar ImGui en el proyecto hay que crear un Inspector y tener ya creado el Input 
```cpp



	MEW::Inspector inspector(w);
	inspector.LinkECS(ecs);

	bucle(){
input.newframe();
		w.newframe(backgroundcolor);
		inspector.NewFrame();
		inspector.update(deltaTime, input);

		//Dibujar el imgui
		inspector.WindowEntities();


inspector.Render();
}
	
```
* Ahora en el motor verás dos ventanas, una con todas las entidades existentes. Si clicamos en una la ventana de la derecha se actualizará con las propiedades de esa entidad  

## X Camara
* Para implementar la cámara en el proyecto hay que crear una camera y tener ya creado el Input la cámara dentro tiene un transform component y un camera component el camera component tiene el tipo de cámara, fov, far,near y el ortho size. 

* Para poder mover la camara como una flycam se requiere que se asignen las acciones que están creadas dentro del camera.hpp 

```cpp

input.assign(MEW::Input::Buttons::KEY_A, MEW::CAMERA_LEFT);
input.assign(MEW::Input::Buttons::KEY_LEFT, MEW::CAMERA_LEFT);
input.assign(MEW::Input::Buttons::KEY_D, MEW::CAMERA_RIGHT);
input.assign(MEW::Input::Buttons::KEY_RIGHT, MEW::CAMERA_RIGHT);
input.assign(MEW::Input::Buttons::KEY_W, MEW::CAMERA_FORWARD);
input.assign(MEW::Input::Buttons::KEY_UP, MEW::CAMERA_FORWARD);
input.assign(MEW::Input::Buttons::KEY_S, MEW::CAMERA_BACK);
input.assign(MEW::Input::Buttons::KEY_DOWN, MEW::CAMERA_BACK);
input.assign(MEW::Input::Buttons::MOUSE_2, MEW::CAMERA_ROTATE);
```
* Al entrar dentro del loop inicial se requiere llamar al camera.update pasandole el delta y el input para que se mueva correctamente.
```cpp

MEW::Camera cameraTest(ecs,640/460);
while (!done) {
	input.newframe();
	w.newframe(backgroundcolor);
	deltaTime = w.deltaTime();
	cameraTest.update(deltaTime, input);
}
```
* Si en algún momento se requiere editar el transform o el camera component de la camara se puede acceder usando un getter que tiene la clase cámara que te devuelve el camera component y el transform component respectivamente.

```cpp

 CameraComponent* GetCameraComponent();
 TransformComponent* GetTransformComp();
```

## X Objetos Y Pintado de ellos

* Para crear objetos va a hacer falta una entidad, un Shader y un Object. 

```cpp

	MEW::Shader shader("../data/exampleLight.vs", "../data/exampleLight.fs");

	MEW::Object objmiku(&shader);
	objmiku.model->loadModel("../data/sponza/sponza.obj");
	objmiku.model->loadMeshes();

	size_t miku = ecs.create_entity("miku");
	ecs.add_component<MEW::RenderComponent>(miku);
	ecs.add_component<MEW::TransformComponent>(miku);
	*ecs.get_component<MEW::RenderComponent>(miku).value().object = objmiku;




bucle(){
//Llamadas que tienen que ir antes del dibujado
		input.newframe();
		w.newframe(backgroundcolor);
		inspector.NewFrame();

		deltaTime = w.deltaTime();

		cameraTest.update(deltaTime, input);
		inspector.update(deltaTime, input);
//Componentes necesarios
		const auto& constVecTransform = ecs.get_vectorComponent<MEW::TransformComponent>();
		const auto& constVecRender = ecs.get_vectorComponent<MEW::RenderComponent>();
		auto& vecL = ecs.get_vectorComponent<MEW::LightComponent>();
		auto vecC = &ecs.get_component<MEW::CameraComponent>(cameraTest.entity_);
		auto vecCT = &ecs.get_component<MEW::TransformComponent>(cameraTest.entity_);


				MEW::RenderSystemLit()(constVecTransform, constVecRender, vecL, shader, vecC, vecCT);


		w.endWindowFrame();
}

```

## X Luces

* Las luces están simplificadas para ser una clase y el propio constructor le facilitase el trabajo al usuario. En el constructor le decimos el tipo de luz que es y se generará una por defecto.

```cpp

MEW::Light directional(ecs, MEW::KTypeLight::Directional);
MEW::Light pointlight(ecs, MEW::KTypeLight::Point);
MEW::Light spotlight(ecs, MEW::KTypeLight::Spot);
MEW::Light ambient(ecs, MEW::KTypeLight::Ambient);

```

* Podremos cambiarle los valores con el inspector y llamando a sus métodos y variables públicas.
