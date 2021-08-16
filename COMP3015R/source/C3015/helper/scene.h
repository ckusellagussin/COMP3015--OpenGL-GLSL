#pragma once

#include <glm/glm.hpp>

class Scene
{
protected:
    // CKG: Scene timer in seconds.
    float m_oldTime, m_deltaTime;

    int m_width;
    int m_height;

    // CKG: Main projection matrix.
    glm::mat4 m_projectionMatrix;

public:

	Scene() : m_animate(true), m_width(800), m_height(600) { }
	virtual ~Scene() {}

	void setDimensions( int w, int h ) {
	    m_width = w;
	    m_height = h;
	}
	
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initScene() = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void update( float t ) = 0;

    /**
      Draw your scene.
      */
    virtual void render() = 0;

    /**
      Called when screen is resized
      */
    virtual void resize(int, int) = 0;
    
    //void animate( bool value ) { m_animate = value; }
    //bool animating() { return m_animate; }

    // CKG: Handle keyboard input.
    virtual void keyCallback(int key, int scancode, int action, int mods) = 0;

protected:
	bool m_animate;
};
