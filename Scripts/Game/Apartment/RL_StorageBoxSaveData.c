[BaseContainerProps()]
class RL_StorageBoxSaveDataClass : EPF_EntitySaveDataClass
{
};

[EDF_DbName.Automatic()]
class RL_StorageBoxSaveData : EPF_EntitySaveData
{
   string m_sOwnerPersistenceID;
   string m_sStorageType;

   //------------------------------------------------------------------------------------------------
   override EPF_EReadResult ReadFrom(IEntity entity, EPF_EntitySaveDataClass attributes)
   {
       EPF_EReadResult readResult = super.ReadFrom(entity, attributes);

       RL_StorageBox_Entity storageEntity = RL_StorageBox_Entity.Cast(entity);
       m_sOwnerPersistenceID = storageEntity.GetOwnerId();
       m_sStorageType = storageEntity.GetStorageType();

       return readResult;
   }

   //------------------------------------------------------------------------------------------------
   override EPF_EApplyResult ApplyTo(IEntity entity, EPF_EntitySaveDataClass attributes)
   {
       EPF_EApplyResult applyResult = super.ApplyTo(entity, attributes);

       RL_StorageBox_Entity storageEntity = RL_StorageBox_Entity.Cast(entity);
       storageEntity.SetOwnerId(m_sOwnerPersistenceID);
       storageEntity.SetStorageType(m_sStorageType);

       return applyResult;
   }

   //------------------------------------------------------------------------------------------------
   override protected bool SerializationSave(BaseSerializationSaveContext saveContext)
   {
       if (!super.SerializationSave(saveContext))
           return false;

       saveContext.WriteValue("m_sOwnerPersistenceID", m_sOwnerPersistenceID);
       saveContext.WriteValue("m_sStorageType", m_sStorageType);

       return true;
   }

   //------------------------------------------------------------------------------------------------
   override protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
   {
       if (!super.SerializationLoad(loadContext))
           return false;

       loadContext.ReadValue("m_sOwnerPersistenceID", m_sOwnerPersistenceID);
       loadContext.ReadValue("m_sStorageType", m_sStorageType);
       
       return true;
   }
};