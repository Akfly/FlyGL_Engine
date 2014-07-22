///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL Mathematics Copyright (c) 2005 - 2014 G-Truc Creation (www.g-truc.net)
///////////////////////////////////////////////////////////////////////////////////////////////////
// Created : 2005-12-21
// Updated : 2009-03-06
// Licence : This source is under MIT License
// File    : glm/gtx/projection.inl
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace glm
{
	template <typename vecType> 
	GLM_FUNC_QUALIFIER vecType proj
	(
		vecType const & x, 
		vecType const & Normal
	)
	{
		return glm::dot(x, Normal) / glm::dot(Normal, Normal) * Normal;
	}
}//namespace glm
