local elapsed = 0
local direction = 1

function on_update(entity, dt)
    elapsed = elapsed + dt
    if elapsed > 1.5 then
        direction = -direction
        elapsed = 0
    end
    entity:get_rigidbody():set_velocity(60 * direction, 0)
end
