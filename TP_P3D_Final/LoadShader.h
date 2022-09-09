#include <GL\gl.h>
#include <GL\glew.h>

//debug
//#define _DEBUG

typedef struct {
	GLenum       type;
	const char* filename;
	GLuint       shader;
} ShaderInfo;

GLuint LoadShaders(ShaderInfo*);

/* -----------------------------------------------------------------------------------
					 GLuint LoadShaders(ShaderInfo*);

Descrição:
----------
Recebe um array de estruturas ShaderInfo.
Cada estrutura contém:
- tipo de shader. No OpenGL poderá ser um dos seguintes valores:
  - GL_COMPUTE_SHADER
  - GL_VERTEX_SHADER
  - GL_TESS_CONTROL_SHADER
  - GL_TESS_EVALUATION_SHADER
  - GL_GEOMETRY_SHADER
  - GL_FRAGMENT_SHADER
- apontador para uma C-string, que contém o nome do ficheiro com código do shader
- valor que referencia o objeto shader criado

--------
Em caso de sucesso, a função retorna o valor que referencia o objeto programa.
Em caso de erro, será retornado o valor zero (0).

*///-----------------------------------------------------------------------------------
