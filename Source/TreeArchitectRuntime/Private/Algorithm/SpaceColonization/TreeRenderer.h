//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#pragma once

//
//
//namespace minitree {
//
//	class TreeData;
//	typedef boost::shared_ptr<TreeData> TreeDataPtr;
//
//	class BranchNode;
//	typedef boost::shared_ptr<BranchNode> BranchNodePtr;
//
//	struct TreeGeometry;
//
//	class GAME_API TreeRenderer : public respawn::renderer::IRenderable {
//	public:
//		TreeRenderer(const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree); 
//		virtual void Draw( const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader );
//		virtual uint32 GetVertexCount() const;
//		virtual const stl::string& GetMaterialName() const;
//		virtual respawn::renderer::AxisAlignedBoundingBox GetObjectAABB() const;
//
//	private:
//		void UploadGeometry(const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader);
//		virtual TreeGeometry& GetGeometry() = 0;
//
//	protected:
//		respawn::renderer::DynamicMeshPtr mesh;
//		respawn::renderer::IndexBufferPtr indexBuffer;
//		TreeDataPtr tree;
//		stl::string materialName;
//		size_t indexSize;
//		bool dirty;
//		bool sortIndices;
//		stl::vector<uint16> frameIndices;
//	};
//
//	class TreeTrunkRenderer : public TreeRenderer {
//	public:
//		TreeTrunkRenderer(const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree);
//
//	private:
//		virtual TreeGeometry& GetGeometry();
//
//	};
//
//	class TreeLeafRenderer : public TreeRenderer {
//	public:
//		TreeLeafRenderer(const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree);
//
//	private:
//		virtual TreeGeometry& GetGeometry();
//
//	};
//
//	class GAME_API TreeDebugRenderer : public respawn::renderer::IRenderable {
//	public:
//		TreeDebugRenderer(const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree);
//		virtual const stl::string& GetMaterialName() const;
//		virtual respawn::renderer::AxisAlignedBoundingBox GetObjectAABB() const;
//		
//	protected:
//		respawn::renderer::DynamicMeshPtr mesh;
//		TreeDataPtr tree;
//		stl::string materialName;
//		bool dirty;
//	};
//
//	class GAME_API TreeDebugRendererSpacePoints : public TreeDebugRenderer {
//	public:
//		TreeDebugRendererSpacePoints(const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree); 
//		virtual void Draw( const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader );
//
//	private:
//
//	};
//
//	class GAME_API TreeDebugRendererBranchNodes : public TreeDebugRenderer {
//	public:
//		TreeDebugRendererBranchNodes(const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree); 
//		virtual void Draw( const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader );
//		void VisitNode(BranchNodePtr node);
//
//	private:
//		void UploadGeometry(const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader);
//
//	private:
//		respawn::renderer::DynamicMeshPtr lineMesh;
//	};
//
//	class GAME_API TreeDebugRendererWireframe : public TreeDebugRenderer {
//	public:
//		TreeDebugRendererWireframe(const respawn::renderer::RendererContext& rendererContext, TreeDataPtr tree); 
//		virtual void Draw( const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader );
//		virtual uint32 GetVertexCount() const;
//
//	private:
//		void UploadGeometry(const respawn::renderer::RendererContext& rendererContext, respawn::renderer::ShaderProgram* shader);
//
//	private:
//		respawn::renderer::IndexBufferPtr indexBuffer;
//		int indexSize;
//
//	};
//}
//
