#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform mat4 Projection;
uniform mat3 NormalMatrix;
uniform float time;

//effect in model
uniform float effectModel;

in vec3 vPosition;			// Coordenadas locais do vértice
in vec3 vNormal;			// Normal do vértice
in vec2 vTextureCoords;  //recebe coordenadas UV

out vec3 vPositionEyeSpace;
out vec3 vNormalEyeSpace;
out vec3 textureVector;
out vec2 textureCoord;   //envia coordenadas UV

void main()
{ 
	// Posição do vértice em coordenadas do olho.
	vPositionEyeSpace = (ModelView * vec4(vPosition, 1.0)).xyz;

	// Transformar a normal do vértice.
	vNormalEyeSpace = normalize(NormalMatrix * vNormal);

	// Coordenada de textura para o CubeMap
	textureVector = vPosition;

	//UV
	textureCoord = vTextureCoords;
	
	if(effectModel != 0)
	{
		// Posição final do vértice (em coordenadas de clip)
		// Posição final do vértice (em coordenadas de clip)
		//gl_position contem a variavel da posisão do vertice 
		//abs é para retornar sempre um valor positivo e nunca negativo 
		//sin() é para retornar sempr eum valor compreendido entre 0 e 1
		//desta forma não precisamos de preocupar com as mormais porque o valor é sempre escalar 
		//caso fosse pro um formato mais tradicional teriamos de usar a inversa da transposta para as normais se manter corretas
		//gl_Position = Projection * ModelView * vec4(vPosition*abs(sin(time + vPosition.y + vPosition.x))/* Deformaçao em funcao do tempo*/, 1.0f);
	
		//gl_Position = Projection * ModelView * vec4(vPosition.x * abs(sin(time)), vPosition.y * abs(sin(time + vPosition.y)), vPosition.z * abs(sin(time)), 1.0f);

		//gl_Position = Projection * ModelView * vec4(vPosition.x, vPosition.y * clamp(abs(sin(time + vPosition.y)), 0.4f, 0.6f), vPosition.z , 1.0f);

		//gl_Position = Projection * ModelView * vec4(vPosition.x, vPosition.y , vPosition.z * clamp(abs(sin(time + vPosition.z)), 0.5f, 0.8f), 1.0f);
	
		//Melhor opção para observar luzes e transmoções
		//gl_Position = Projection * ModelView * vec4(vPosition.x  * clamp(abs(sin(time + vPosition.y)), 0.3f, 0.7f), vPosition.y , vPosition.z, 1.0f);
	
		//não fica estático no lugar por isso as luzes deixam de afetar 
		//melhor transformação
		gl_Position = Projection * ModelView * vec4(vPosition.x  + clamp(sin(time + vPosition.y), -0.1f, 0.1f), vPosition.y , vPosition.z, 1.0f);

		//gl_Position = Projection * ModelView * vec4(vPosition*abs(sin(time))/* Deformaçao em funcao do tempo*/, 1.0f);	
		//gl_Position = Projection * ModelView * vec4(vPosition + abs(time * sin(vPosition.y)), 1.0f);

		//para fazer uma transformação na vertical e na horizontal provavelmente teria de ser aqui tb 
		//era usar a mesma estratégia só que com uma matriz alterada 
		//depois nao sei se temos aqui a funcao da inversa da transposta 
		//ou teriamos de a calcular manualmente 
		//seria interessante fazer isso 
		//PS: O prof n quer isto 
	}
	else
	{
		//Normal representation
		gl_Position = Projection * ModelView * vec4(vPosition, 1.0f);
	}


}
