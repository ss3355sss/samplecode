#pragma once
#include "lib/baseMaya/pch.h"

#include "lib/base/logger.h"
#include "maya/MFnPlugin.h"
#include "maya/MDrawRegistry.h"

NAMESPACE_BASE_BEGIN

class PluginRegister final
{
public:
	struct Plugin
	{
	public:
		virtual MStatus registerPlugin(MFnPlugin& pluginFn)
		{
			return MS::kFailure;
		}
		virtual MStatus deregisterPlugin(MFnPlugin& pluginFn)
		{
			return MS::kFailure;
		}
	};
	struct MPxData : public Plugin
	{
	public:
		MString          typeName;
		MTypeId          typeId;
		MCreatorFunction creatorFunction;
	public:
		MPxData(const MString typeName,
		        const MTypeId typeId,
		        const MCreatorFunction creatorFunction)
			: typeName(typeName),
			  typeId(typeId),
			  creatorFunction(creatorFunction)
		{
		}
		virtual ~MPxData()
		{
		}
	public:
		virtual MStatus registerPlugin(MFnPlugin& pluginFn)
		{
			return pluginFn.registerData(typeName,
			                             typeId,
			                             creatorFunction,
			                             ::MPxData::kData
			);
		}
		virtual MStatus deregisterPlugin(MFnPlugin& pluginFn)
		{
			return pluginFn.deregisterData(typeId);
		}
	};
	struct MPxGemoetryData : public Plugin
	{
	public:
		MString          typeName;
		MTypeId          typeId;
		MCreatorFunction creatorFunction;
	public:
		MPxGemoetryData(const MString typeName,
		        const MTypeId typeId,
		        const MCreatorFunction creatorFunction)
			: typeName(typeName),
			  typeId(typeId),
			  creatorFunction(creatorFunction)
		{
		}
		virtual ~MPxGemoetryData()
		{
		}
	public:
		virtual MStatus registerPlugin(MFnPlugin& pluginFn)
		{
			return pluginFn.registerData(typeName,
			                             typeId,
			                             creatorFunction,
			                             ::MPxData::kGeometryData
			);
		}
		virtual MStatus deregisterPlugin(MFnPlugin& pluginFn)
		{
			return pluginFn.deregisterData(typeId);
		}
	};
	struct MPxNode : public Plugin
	{
	public:
		MString             typeName;
		MTypeId             typeId;
		MCreatorFunction    creatorFunction;
		MInitializeFunction initFunction;
	public:
		MPxNode(const MString typeName,
		        const MTypeId typeId,
		        const MCreatorFunction creatorFunction,
		        const MInitializeFunction initFunction)
			: typeName(typeName),
			  typeId(typeId),
			  creatorFunction(creatorFunction),
			  initFunction(initFunction)
		{
		}
		virtual ~MPxNode()
		{
		}
	public:
		virtual MStatus registerPlugin(MFnPlugin& pluginFn)
		{
			return pluginFn.registerNode(typeName,
			                             typeId,
			                             creatorFunction,
			                             initFunction,
			                             ::MPxNode::kDependNode
			);
		}
		virtual MStatus deregisterPlugin(MFnPlugin& pluginFn)
		{
			return pluginFn.deregisterNode(typeId);
		}
	};
	struct MPxTransformNode : public Plugin
	{
	public:
		MString                                       typeName;
		MTypeId                                       typeId;
		MCreatorFunction                              creatorFunction;
		MInitializeFunction                           initFunction;
		MCreatorFunction                              xformCreatorFunction;
		MTypeId                                       xformId;
		MString                                       drawClassification;
	public:
		MPxTransformNode(const MString typeName,
		                 const MTypeId typeId,
		                 const MCreatorFunction creatorFunction,
		                 const MInitializeFunction initFunction,
		                 const MCreatorFunction xformCreatorFunction,
		                 const MTypeId xformId,
		                 const MString drawClassification)
		: typeName(typeName),
		  typeId(typeId),
		  creatorFunction(creatorFunction),
		  initFunction(initFunction),
		  xformCreatorFunction(xformCreatorFunction),
		  xformId(xformId),
		  drawClassification(drawClassification)
		{
		}
		virtual ~MPxTransformNode()
		{
		}
	public:
		virtual MStatus registerPlugin(MFnPlugin& pluginFn)
		{
			if(drawClassification.length() > 0)
			{
				return pluginFn.registerTransform(typeName,
				                                  typeId,
				                                  creatorFunction,
				                                  initFunction,
				                                  xformCreatorFunction,
				                                  xformId,
				                                  &drawClassification
				);
			}
			return pluginFn.registerTransform(typeName,
			                                  typeId,
			                                  creatorFunction,
			                                  initFunction,
			                                  xformCreatorFunction,
			                                  xformId
			);
		}
		virtual MStatus deregisterPlugin(MFnPlugin& pluginFn)
		{
			return pluginFn.deregisterNode(typeId);
		}
	};
	struct MPxShape : public Plugin
	{
	public:
		typedef Plugin Base;
	public:
		MString             typeName;
		MTypeId             typeId;
		MCreatorFunction    creatorFunction;
		MInitializeFunction initFunction;
		MCreatorFunction    uiCreatorFunction;
		MString             drawClassification;
	public:
		MPxShape(const MString typeName,
		         const MTypeId typeId,
		         const MCreatorFunction creatorFunction,
		         const MInitializeFunction initFunction,
		         const MCreatorFunction uiCreatorFunction,
		         const MString drawClassification)
		: typeName(typeName),
		  typeId(typeId),
		  creatorFunction(creatorFunction),
		  initFunction(initFunction),
		  uiCreatorFunction(uiCreatorFunction),
		  drawClassification(drawClassification)
		
		{
		}
		virtual ~MPxShape()
		{
		}
	public:
		virtual MStatus registerPlugin(MFnPlugin& pluginFn)
		{
			if(drawClassification.length() > 0)
			{
				return pluginFn.registerShape(typeName,
				                              typeId,
				                              creatorFunction,
				                              initFunction,
				                              uiCreatorFunction,
				                              &drawClassification
				);
			}
			return pluginFn.registerShape(typeName,
			                              typeId,
			                              creatorFunction,
			                              initFunction,
			                              uiCreatorFunction
			);
		}
		virtual MStatus deregisterPlugin(MFnPlugin& pluginFn)
		{
			return pluginFn.deregisterNode(typeId);
		}
	};
	struct MPxDrawOverride : public Plugin
	{
	public:
		typedef Plugin Base;
	public:
		MString                                         drawClassfication;
		MString                                         registrantId;
		MHWRender::MDrawRegistry::DrawOverrideCreator   creator;
	public:
		MPxDrawOverride(const MString drawClassfication,
		                const MString registrantId,
		                const MHWRender::MDrawRegistry::DrawOverrideCreator creator)
			: drawClassfication(drawClassfication),
			  registrantId(registrantId),
			  creator(creator)
		{
		}
		MPxDrawOverride(const MString drawClassfication)
			: drawClassfication(drawClassfication)
		{
		}

		virtual ~MPxDrawOverride()
		{
		}

	public:
		virtual MStatus registerPlugin(MFnPlugin& pluginFn)
		{
			return MHWRender::MDrawRegistry::registerDrawOverrideCreator(drawClassfication,
			                                                             registrantId,
			                                                             creator
			);

		}
		virtual MStatus deregisterPlugin(MFnPlugin& pluginFn)
		{
			return MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(drawClassfication,
			                                                               registrantId
			);

		}
	};
	struct MPxGeometryOverride : public Plugin
	{
	public:
		MString                                           drawClassfication;
		MString                                           registrantId;
		MHWRender::MDrawRegistry::GeometryOverrideCreator creator;
	public:
		MPxGeometryOverride(const MString drawClassfication,
		                    const MString registrantId,
		                    const MHWRender::MDrawRegistry::GeometryOverrideCreator creator)
		: drawClassfication(drawClassfication),
		  registrantId(registrantId),
		  creator(creator)
		{
		}
		virtual ~MPxGeometryOverride()
		{
		}
		
	public:
		virtual MStatus registerPlugin(MFnPlugin& pluginFn)
		{
			return MHWRender::MDrawRegistry::registerGeometryOverrideCreator(drawClassfication,
			                                                                 registrantId,
			                                                                 creator
			);
			
		}
		virtual MStatus deregisterPlugin(MFnPlugin& pluginFn)
		{
			return MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator(drawClassfication,
			                                                                   registrantId
			);
			
		}
	};
	struct MPxComponentConverter : public Plugin
	{
	public:
		MString                                             renderItem;
		MHWRender::MDrawRegistry::ComponentConverterCreator creator;
	public:
		MPxComponentConverter(const MString renderItem,
		                      const MHWRender::MDrawRegistry::ComponentConverterCreator creator)
			: renderItem(renderItem),
			  creator(creator)
		{
		}
		virtual ~MPxComponentConverter()
		{
		}
	
	public:
		virtual MStatus registerPlugin(MFnPlugin& pluginFn)
		{
			MStatus stat;
			stat = MHWRender::MDrawRegistry::registerComponentConverter(renderItem,
			                                                            creator
			);
			return stat;
			
		}
		virtual MStatus deregisterPlugin(MFnPlugin& pluginFn)
		{
			return MHWRender::MDrawRegistry::deregisterComponentConverter(renderItem);
			
		}
	};
	struct MPxCommand : public Plugin
	{
	public:
		MString               commandName;
		MCreatorFunction      creatorFunction;
		MCreateSyntaxFunction createSyntaxFunction;
	public:
		MPxCommand(const MString commandName,
		           const MCreatorFunction creatorFunction,
		           const MCreateSyntaxFunction createSyntaxFunction)
			: commandName(commandName),
			  creatorFunction(creatorFunction),
			  createSyntaxFunction(createSyntaxFunction)
		{
		}
		virtual ~MPxCommand()
		{
		}
	public:
		virtual MStatus registerPlugin(MFnPlugin& pluginFn)
		{
			return pluginFn.registerCommand(commandName,
			                                creatorFunction,
			                                createSyntaxFunction
			);
		}
		virtual MStatus deregisterPlugin(MFnPlugin& pluginFn)
		{
			return pluginFn.deregisterCommand(commandName);
		}
		
	};
	
public:
	static std::shared_ptr<PluginRegister> Get()
	{
		static std::shared_ptr<PluginRegister> pRegister_ = nullptr;
		if(!pRegister_)
			pRegister_ = std::make_shared<PluginRegister>();
		return pRegister_;
	}
	static MString GetIdentifier(const std::shared_ptr<Plugin> pPlugin)
	{
		MString identifier;
		if(std::shared_ptr<MPxData> pDerived = std::dynamic_pointer_cast<MPxData>(pPlugin))
			identifier = pDerived->typeName;
		if(std::shared_ptr<MPxGemoetryData> pDerived = std::dynamic_pointer_cast<MPxGemoetryData>(pPlugin))
			identifier = pDerived->typeName;
		if(std::shared_ptr<MPxNode> pDerived = std::dynamic_pointer_cast<MPxNode>(pPlugin))
			identifier = pDerived->typeName;
		if(std::shared_ptr<MPxTransformNode> pDerived = std::dynamic_pointer_cast<MPxTransformNode>(pPlugin))
			identifier = pDerived->typeName;
		if(std::shared_ptr<MPxShape> pDerived = std::dynamic_pointer_cast<MPxShape>(pPlugin))
			identifier = pDerived->typeName;
		if(std::shared_ptr<MPxGeometryOverride> pDerived = std::dynamic_pointer_cast<MPxGeometryOverride>(pPlugin))
			identifier = pDerived->drawClassfication;
		if(std::shared_ptr<MPxDrawOverride> pDerived = std::dynamic_pointer_cast<MPxDrawOverride>(pPlugin))
			identifier = pDerived->drawClassfication;
		if(std::shared_ptr<MPxComponentConverter> pDerived = std::dynamic_pointer_cast<MPxComponentConverter>(pPlugin))
			identifier = pDerived->renderItem;
		if(std::shared_ptr<MPxCommand> pDerived = std::dynamic_pointer_cast<MPxCommand>(pPlugin))
			identifier = pDerived->commandName;
			
		return identifier;
	}

private:
	std::stack<std::shared_ptr<Plugin>>  registered_;

public:
	PluginRegister()
	{
	}
	~PluginRegister()
	{
	}
public:
	MStatus registerPlugin(MFnPlugin& pluginFn, const std::shared_ptr<Plugin> pPlugin)
	{
		DECL_LOGGER(false);
		MStatus stat;
		
		MString identifier = PluginRegister::GetIdentifier(pPlugin);
		LOGGER_INFO("register \"%s\"", identifier.asChar());
		stat = pPlugin->registerPlugin(pluginFn);
		if(stat == MS::kSuccess)
		{
			registered_.push(pPlugin);
		}
		else
		{
			LOGGER_ERROR("Failed to register \"%s\"", identifier.asChar());
			stat = deregisterPlugin(pluginFn);
			return MS::kFailure;
		}
		return stat;
	}
	MStatus deregisterPlugin(MFnPlugin& pluginFn)
	{
		DECL_LOGGER(false);
		
		MStatus stat;
		while(!registered_.empty())
		{
			std::shared_ptr<Plugin> pPlugin = registered_.top();
			MString identifier = PluginRegister::GetIdentifier(pPlugin);
			LOGGER_INFO("deregister \"%s\"", identifier.asChar());
			stat = pPlugin->deregisterPlugin(pluginFn);
			if(stat != MS::kSuccess)
			{
				LOGGER_WARNING("Failed to deregister \"%s\"", identifier.asChar());
			}
			registered_.pop();
		}
		return stat;
	}
};
NAMESPACE_BASE_END
