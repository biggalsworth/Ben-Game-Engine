using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Braveheart
{
    public enum State
    {
        Passive,
        Detected,
        Attacking
    }

    public class EnemyDetection : GameObject
    {

        public float detectionRange = 4.0f;
        public float maxSpeed = 0.75f;
        State state;

        GameObject Player;

        Vector3 pos;

        Vector4 UndetectedColour;

        float shotdelay = 2.0f;
        float elapsed = 0.0f;

        public EnemyDetection() : base ()
        {

        }

        public void Start()
        {
            Player = World.FindEntityByName("Player");
            UndetectedColour = GetComponent<SpriteRendererComponent>().Colour;
            Debug.Log("COLOUR : " + UndetectedColour.ToString());

        }

        public void Update(float delta)
        {

            TransformComponent trans = GetComponent<TransformComponent>();
            pos = trans.Translation;

            CheckDetection();

            RespondToAI();

            //to keep track of shot timings
            if (elapsed <= shotdelay)
            {
                elapsed += delta;
            }

        }



        public void CheckDetection()
        {
            uint[] hits = Physics2D.CircleCast(pos, 0.0f, detectionRange);
            state = State.Passive;
            
            foreach(uint hitID in hits)
            {
                if (hitID != this.id)
                {
                    if (hitID.Equals(Player.id))
                    {

                        state = State.Detected;
            
                    }
                }
            }
            
            if(state.Equals(State.Passive))
            {
                GetComponent<SpriteRendererComponent>().Colour = new Vector4(1.0f, 0.824f, 0.333f, 1.0f);
            }
            else
            {
                GetComponent<SpriteRendererComponent>().Colour = Colour.Red;
            }

        }


        public void RespondToAI()
        {
            Vector3 dist = Player.GetComponent<TransformComponent>().Translation - pos;

            if (state == State.Passive)
            {
                return;
            }
            else
            {
                if(dist.Magnitude() > detectionRange + 1)
                {
                    state = State.Passive;
                }
            }
            if (state == State.Detected)
            {

                if (dist.Magnitude() <= detectionRange - 0.75f)
                {
                    state = State.Attacking;
                }
                else if (GetComponent<Rigidbody2DComponent>().LinearVelocity.Magnitude() < maxSpeed)
                {
                    Vector3 dir = dist.Normalise();
                    GetComponent<Rigidbody2DComponent>().ApplyLinearImpulseToCentre(dir * 0.45f);
                }
            }

            if(state == State.Attacking && elapsed >= shotdelay)
            {
                elapsed = 0.0f;
                GetComponent<AudioSourceComponent>().PlayOneShot();

                Vector3 shot = pos;
                Vector3 direction = dist;
                
                //shot += direction * 5f;
                shot += dist.Normalise() * 1.5f;

                RayCastHit hit = Physics2D.RayCast(pos, Player.GetComponent<TransformComponent>().Translation);
                if(hit.count > 0 && hit.shapeId.Equals(Player.id))
                {
                    Debug.Log("Self ID: " + this.id.ToString());
                    Debug.Log("Player ID: " + Player.id.ToString());
                    Player.GetComponent<SpriteRendererComponent>().Colour = new Vector4(1.0f, 0.824f, 0.333f, 1.0f);//.ApplyLinearImpulseToCentre(direction * 1.5f);
                    Player.GetComponent<Rigidbody2DComponent>().ApplyLinearImpulseToCentre(new Vector3(direction * 1.5f));

                    PlayerStats stats = new GameObject(hit.shapeId).GetScript<PlayerStats>();
                    stats.Damage(10.0f);

                }

                //GetComponent<Rigidbody2DComponent>().ApplyLinearImpulseToCentre(direction.Normalise() * -1.5f);

            }

        }
    }
}


