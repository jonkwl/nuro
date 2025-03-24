#pragma once

#include <cstdint>

#include <viewport/viewport.h>
#include <rendering/shader/shader.h>
#include <memory/resource_manager.h>

/* !!!!!!!!!!!!!!!!!!!!!!!!!!
   !!					   !!
   !! BAD TEMPORARY CODE!  !!
   !!					   !!
   !!!!!!!!!!!!!!!!!!!!!!!!!! */

class IMaterial
{
public:
	virtual ~IMaterial() {}

	virtual void bind() const = 0;
	virtual uint32_t getId() const = 0;
	virtual ResourceRef<Shader> getShader() const = 0;
	virtual uint32_t getShaderId() const = 0;
};