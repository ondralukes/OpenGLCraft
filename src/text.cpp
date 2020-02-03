#include "text.hpp"

void
TextManager::init(const char * texturePath){
  textureID = loadDDS(texturePath);

  glGenBuffers(1, &verticesBuffer);
	glGenBuffers(1, &UVBuffer);

  shaderID = LoadShaders("shaders/gui.vertexshader", "shaders/gui.fragmentshader");
  uniformID = glGetUniformLocation(shaderID, "textureSampler");
}

void
TextManager::drawText(const char * text, glm::vec2 pos, float size){
  std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;
	for ( unsigned int i=0 ; i<strlen(text) ; i++ ){

		glm::vec2 vertex_ul  = glm::vec2( pos.x+i*size     , pos.y+size );
		glm::vec2 vertex_ur  = glm::vec2( pos.x+i*size+size, pos.y+size );
		glm::vec2 vertex_dr  = glm::vec2( pos.x+i*size+size, pos.y      );
		glm::vec2 vertex_dl  = glm::vec2( pos.x+i*size     , pos.y      );

		vertices.push_back(vertex_ul   );
		vertices.push_back(vertex_dl );
		vertices.push_back(vertex_ur  );

		vertices.push_back(vertex_dr);
		vertices.push_back(vertex_ur);
		vertices.push_back(vertex_dl);

		char c = text[i];
		float uv_x = (c%16)/16.0f;
		float uv_y = (c/16)/16.0f;

		glm::vec2 uv_ul    = glm::vec2( uv_x           , uv_y );
		glm::vec2 uv_ur   = glm::vec2( uv_x+1.0f/16.0f, uv_y );
		glm::vec2 uv_dr = glm::vec2( uv_x+1.0f/16.0f, (uv_y + 1.0f/16.0f) );
		glm::vec2 uv_dl  = glm::vec2( uv_x           , (uv_y + 1.0f/16.0f) );
		UVs.push_back(uv_ul   );
		UVs.push_back(uv_dl );
		UVs.push_back(uv_ur  );

		UVs.push_back(uv_dr);
		UVs.push_back(uv_ur);
		UVs.push_back(uv_dl);
	}
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

	// Bind shader
	glUseProgram(shaderID);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glUniform1i(uniformID, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
