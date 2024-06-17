

// Borrow 함수는 먼저 해당 fbx 이름에 맞는 풀이 존재하는지 확인하고, 없으면 새로 만든다.
// 해당 풀이 있다면 
//std::weak_ptr<Projectile> ProjectilePool::Borrow(std::weak_ptr<Unit> owner, Vector3d destination, ProjectileType::Enum projectileType, ProjectileHoming::Enum projectileHoming)
//{
//    const std::string& fbxname = owner.lock()->GetUnitTemplateData().pod.projectile_staticFBXName;
//    if (!poolsByFBX.contains(fbxname))
//    {
//        auto pool = std::make_shared<PoolByMesh>();
//        pool->fbxname = fbxname;
//        poolsByFBX[fbxname] = pool;
//    }
//    auto ret = poolsByFBX[fbxname]->Borrow();
//    ret.lock()->owner = owner;
//    const auto& v = owner.lock()->GetUnitTemplateData().pod.projectileOffset;
//    DirectX::XMVECTOR offset = DirectX::XMVectorSet(v.x, v.y, v.z, 1);
//    auto yunuWtm = owner.lock()->GetTransform()->GetWorldTM();
//    DirectX::XMMATRIX wtm = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&yunuWtm));
//    offset = DirectX::XMVector4Transform(offset, wtm);
//    ret.lock()->GetTransform()->SetWorldPosition({ offset.m128_f32[0],offset.m128_f32[1],offset.m128_f32[2] });
//
//    Vector3d horizontalDisplacement = destination - ret.lock()->GetTransform()->GetWorldPosition();
//    horizontalDisplacement.y = 0;
//    ret.lock()->speed = horizontalDisplacement.Normalized() * owner.lock()->GetUnitTemplateData().pod.projectileSpeed;
//    float t = horizontalDisplacement.Magnitude() / ret.lock()->speed.Magnitude();
//    float y0 = ret.lock()->GetTransform()->GetWorldPosition().y;
//    float vy0 = (-y0 + 0.5f * GlobalConstant::GetSingletonInstance().pod.gravitySpeed * t * t) / t;
//
//    ret.lock()->speed += Vector3d::up * vy0;
//    ret.lock()->damage = owner.lock()->GetUnitTemplateData().pod.m_autoAttackDamage + owner.lock()->adderAttackDamage;
//    ret.lock()->traveling = true;
//    return ret;
//}
//
//void ProjectilePool::Return(std::weak_ptr<Projectile> projectile)
//{
//    if (projectile.expired())
//    {
//        return;
//    }
//    projectile.lock()->traveling = false;
//    poolsByProjectile.at(projectile.lock().get()).lock()->Return(projectile);
//}
//
//void ProjectilePool::PoolByMesh::ObjectInitializer(std::weak_ptr<Projectile> projectile)
//{
//    auto gameObj = Scene::getCurrentScene()->AddGameObjectFromFBX(fbxname);
//    gameObj->SetParent(projectile.lock()->GetGameObject());
//    ProjectilePool::SingleInstance().poolsByProjectile[projectile.lock().get()] = ProjectilePool::SingleInstance().poolsByFBX.at(fbxname);
//    projectile.lock()->SetRadius(0.0001);
//}
