#pragma once

#include "ShaderUtils.h"

#include <GL/gl.h>

namespace CSCI441 {

	class ShaderProgram {
	public:
		ShaderProgram( const char *vertexShaderFilename,
					   const char *fragmentShaderFilename );

		ShaderProgram( const char *vertexShaderFilename,
					   const char *tesselationControlShaderFilename,
					   const char *tesselationEvaluationShaderFilename,
					   const char *geometryShaderFilename,
					   const char *fragmentShaderFilename );

		ShaderProgram( const char *vertexShaderFilename,
					   const char *geometryShaderFilename,
					   const char *fragmentShaderFilename );

		~ShaderProgram();

		GLint getUniformLocation( const char *uniformName );
		GLint getAttributeLocation( const char *attributeName );
		GLuint getSubroutineIndex( GLenum shaderStage, const char *subroutineName );

		GLuint getNumUniforms();
		GLuint getNumAttributes();

		void useProgram();
	private:
		ShaderProgram();

		GLuint _vertexShaderHandle;
		GLuint _tesselationControlShaderHandle;
		GLuint _tesselationEvaluationShaderHandle;
		GLuint _geometryShaderHandle;
		GLuint _fragmentShaderHandle;

		GLuint _shaderProgramHandle;

		bool registerShaderProgram( const char *vertexShaderFilename,
									const char *tesselationControlShaderFilename,
									const char *tesselationEvaluationShaderFilename,
									const char *geometryShaderFilename,
									const char *fragmentShaderFilename );
	};

}
